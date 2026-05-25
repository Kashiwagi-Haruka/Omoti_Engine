import bpy
import math
import gpu
import copy
import bpy_extras
import gpu_extras.batch


bl_info = {
    "name": "レベルエディタ",
    "author": "Taro Kamata",
    "version": (1, 0),
    "blender": (3, 3, 1),
    "location": "",
    "description": "レベルエディタ",
    "warning": "",
    "support": "TESTING",
    "wiki_url": "",
    "tracker_url": "",
    "category": "Object"
}


# =========================================================
# コライダー描画
# =========================================================
class DrawCollider:
    # 3Dビューに登録した描画関数のハンドル
    handle = None

    @staticmethod
    def draw_collider():
        # 頂点データ
        vertices = {
            "pos": []
        }

        # インデックスデータ
        indices = []

        # Boxの8頂点分のローカル座標オフセット
        offsets = [
            [-0.5, -0.5, -0.5],  # 左下前
            [+0.5, -0.5, -0.5],  # 右下前
            [-0.5, +0.5, -0.5],  # 左上前
            [+0.5, +0.5, -0.5],  # 右上前

            [-0.5, -0.5, +0.5],  # 左下奥
            [+0.5, -0.5, +0.5],  # 右下奥
            [-0.5, +0.5, +0.5],  # 左上奥
            [+0.5, +0.5, +0.5],  # 右上奥
        ]

        # 立方体のX,Y,Z方向サイズ
        size = [2, 2, 2]

        # 現在シーン内にある全オブジェクトを走査
        for object in bpy.context.scene.objects:

            # このオブジェクトのBox頂点を追加する前の頂点数
            start = len(vertices["pos"])

            # Boxの8頂点分回す
            for offset in offsets:
                # オブジェクトの中心座標をコピー
                pos = copy.copy(object.location)

                # 中心点を基準に各頂点ごとにずらす
                pos[0] += offset[0] * size[0]
                pos[1] += offset[1] * size[1]
                pos[2] += offset[2] * size[2]

                # 頂点データリストに座標を追加
                vertices["pos"].append(pos)

            # 前面を構成する辺の頂点インデックス
            indices.append([start + 0, start + 1])
            indices.append([start + 2, start + 3])
            indices.append([start + 0, start + 2])
            indices.append([start + 1, start + 3])

            # 奥面を構成する辺の頂点インデックス
            indices.append([start + 4, start + 5])
            indices.append([start + 6, start + 7])
            indices.append([start + 4, start + 6])
            indices.append([start + 5, start + 7])

            # 手前と奥を繋ぐ辺の頂点インデックス
            indices.append([start + 0, start + 4])
            indices.append([start + 1, start + 5])
            indices.append([start + 2, start + 6])
            indices.append([start + 3, start + 7])

        # ビルトインの単色シェーダーを取得
        shader = gpu.shader.from_builtin("UNIFORM_COLOR")

        # 描画用バッチを作成
        batch = gpu_extras.batch.batch_for_shader(
            shader,
            "LINES",
            vertices,
            indices=indices
        )

        # シェーダーのパラメータ設定
        color = [0.5, 1.0, 1.0, 1.0]

        shader.bind()
        shader.uniform_float("color", color)

        # 描画
        batch.draw(shader)


# オペレーター：頂点を伸ばす
class MYADDON_OT_stretch_vertex(bpy.types.Operator):
    bl_idname = "myaddon.myaddon_ot_stretch_vertex"
    bl_label = "頂点を伸ばす"
    bl_description = "頂点座標を引っ張って伸ばします"
    bl_options = {'REGISTER', 'UNDO'}

    def execute(self, context):
        bpy.data.objects["Cube"].data.vertices[0].co.x += 1.0
        print("頂点を伸ばしました。")
        return {'FINISHED'}


# オペレーター：ICO球生成
class MYADDON_OT_create_ico_sphere(bpy.types.Operator):
    bl_idname = "myaddon.myaddon_ot_create_object"
    bl_label = "ICO球生成"
    bl_description = "ICO球を生成します"
    bl_options = {'REGISTER', 'UNDO'}

    def execute(self, context):
        bpy.ops.mesh.primitive_ico_sphere_add()
        print("ICO球を生成しました。")
        return {'FINISHED'}


# オペレーター：file_nameカスタムプロパティ追加
class MYADDON_OT_add_filename(bpy.types.Operator):
    bl_idname = "myaddon.myaddon_ot_add_filename"
    bl_label = "FileName 追加"
    bl_description = "['file_name']カスタムプロパティを追加します"
    bl_options = {'REGISTER', 'UNDO'}

    def execute(self, context):
        if context.object is None:
            self.report({'WARNING'}, "オブジェクトが選択されていません")
            return {'CANCELLED'}

        context.object["file_name"] = ""
        print("file_name を追加しました。")
        return {'FINISHED'}


# オペレーター：シーン出力
class MYADDON_OT_export_scene(
    bpy.types.Operator,
    bpy_extras.io_utils.ExportHelper
):
    bl_idname = "myaddon.myaddon_ot_export_scene"
    bl_label = "シーン出力"
    bl_description = "シーン情報をExportします"
    bl_options = {'REGISTER', 'UNDO'}

    filename_ext = ".scene"

    def write_and_print(self, file, text):
        print(text)
        file.write(text)
        file.write("\n")

    def parse_scene_recursive(self, file, obj, level):
        indent = ""
        for i in range(level):
            indent += "\t"

        self.write_and_print(file, indent + obj.type + " - " + obj.name)

        trans, rot, scale = obj.matrix_local.decompose()
        rot = rot.to_euler()

        rot.x = math.degrees(rot.x)
        rot.y = math.degrees(rot.y)
        rot.z = math.degrees(rot.z)

        self.write_and_print(
            file,
            indent + "Trans(%f,%f,%f)" % (trans.x, trans.y, trans.z)
        )
        self.write_and_print(
            file,
            indent + "Rot(%f,%f,%f)" % (rot.x, rot.y, rot.z)
        )
        self.write_and_print(
            file,
            indent + "Scale(%f,%f,%f)" % (scale.x, scale.y, scale.z)
        )

        if "file_name" in obj:
            self.write_and_print(
                file,
                indent + "FileName(%s)" % obj["file_name"]
            )

        self.write_and_print(file, indent + "END")
        self.write_and_print(file, "")

        for child in obj.children:
            self.parse_scene_recursive(file, child, level + 1)

    def export(self):
        print("シーン情報出力開始... %r" % self.filepath)

        with open(self.filepath, "w", encoding="utf-8") as file:
            self.write_and_print(file, "SCENE")

            for obj in bpy.context.scene.objects:
                if obj.parent:
                    continue

                self.parse_scene_recursive(file, obj, 0)

    def execute(self, context):
        print("シーン情報をExportします")

        self.export()

        self.report({'INFO'}, "シーン情報をExportしました")
        print("シーン情報をExportしました")

        return {'FINISHED'}


# オブジェクトのファイルネームパネル
class OBJECT_PT_file_name(bpy.types.Panel):
    bl_idname = "OBJECT_PT_file_name"
    bl_label = "FileName"
    bl_space_type = "PROPERTIES"
    bl_region_type = "WINDOW"
    bl_context = "object"

    def draw(self, context):
        if context.object is None:
            return

        if "file_name" in context.object:
            self.layout.prop(context.object, '["file_name"]', text=self.bl_label)
        else:
            self.layout.operator(MYADDON_OT_add_filename.bl_idname)


# トップバーの拡張メニュー
class TOPBAR_MT_my_menu(bpy.types.Menu):
    bl_idname = "TOPBAR_MT_my_menu"
    bl_label = "MyMenu"
    bl_description = "拡張メニュー by " + bl_info["author"]

    def draw(self, context):
        self.layout.operator(
            MYADDON_OT_stretch_vertex.bl_idname,
            text=MYADDON_OT_stretch_vertex.bl_label
        )

        self.layout.operator(
            MYADDON_OT_create_ico_sphere.bl_idname,
            text=MYADDON_OT_create_ico_sphere.bl_label
        )

        self.layout.operator(
            MYADDON_OT_export_scene.bl_idname,
            text=MYADDON_OT_export_scene.bl_label
        )

        self.layout.separator()

        self.layout.operator(
            "wm.url_open_preset",
            text="Manual",
            icon='HELP'
        )

    def submenu(self, context):
        self.layout.menu(TOPBAR_MT_my_menu.bl_idname)


# Blenderに登録するクラスリスト
classes = (
    MYADDON_OT_stretch_vertex,
    MYADDON_OT_create_ico_sphere,
    MYADDON_OT_export_scene,
    MYADDON_OT_add_filename,
    OBJECT_PT_file_name,
    TOPBAR_MT_my_menu,
)


# Add-On有効化時コールバック
def register():
    for cls in classes:
        bpy.utils.register_class(cls)

    bpy.types.TOPBAR_MT_editor_menus.append(TOPBAR_MT_my_menu.submenu)

    # 3Dビューに描画関数を追加
    if DrawCollider.handle is None:
        DrawCollider.handle = bpy.types.SpaceView3D.draw_handler_add(
            DrawCollider.draw_collider,
            (),
            "WINDOW",
            "POST_VIEW"
        )

    print("レベルエディタが有効化されました。")


# Add-On無効化時コールバック
def unregister():
    bpy.types.TOPBAR_MT_editor_menus.remove(TOPBAR_MT_my_menu.submenu)

    # 3Dビューから描画関数を削除
    if DrawCollider.handle is not None:
        bpy.types.SpaceView3D.draw_handler_remove(
            DrawCollider.handle,
            "WINDOW"
        )
        DrawCollider.handle = None

    for cls in reversed(classes):
        bpy.utils.unregister_class(cls)

    print("レベルエディタが無効化されました。")


# テスト実行用コード
if __name__ == "__main__":
    register()