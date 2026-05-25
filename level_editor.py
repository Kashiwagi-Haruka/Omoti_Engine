import bpy
import math
import gpu
import copy
import bpy_extras
import gpu_extras.batch
import mathutils


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
    handle = None

    @staticmethod
    def draw_collider():

        vertices = {
            "pos": []
        }

        indices = []

        offsets = [
            [-0.5, -0.5, -0.5],
            [+0.5, -0.5, -0.5],
            [-0.5, +0.5, -0.5],
            [+0.5, +0.5, -0.5],

            [-0.5, -0.5, +0.5],
            [+0.5, -0.5, +0.5],
            [-0.5, +0.5, +0.5],
            [+0.5, +0.5, +0.5],
        ]

        # 現在シーンのオブジェクトを走査
        for object in bpy.context.scene.objects:

            # colliderプロパティがなければ描画しない
            if "collider" not in object:
                continue

            # BOX以外は今のところ非対応
            if object["collider"] != "BOX":
                continue

            # 追加前の頂点数
            start = len(vertices["pos"])

            # デフォルト値
            center = mathutils.Vector((0, 0, 0))
            size = mathutils.Vector((2, 2, 2))

            # カスタムプロパティ取得
            if "collider_center" in object:
                center[0] = object["collider_center"][0]
                center[1] = object["collider_center"][1]
                center[2] = object["collider_center"][2]

            if "collider_size" in object:
                size[0] = object["collider_size"][0]
                size[1] = object["collider_size"][1]
                size[2] = object["collider_size"][2]

            # Box8頂点生成
            for offset in offsets:

                # ローカル座標
                pos = copy.copy(center)

                pos[0] += offset[0] * size[0]
                pos[1] += offset[1] * size[1]
                pos[2] += offset[2] * size[2]

                # ワールド座標へ変換
                pos = object.matrix_world @ pos

                # 頂点追加
                vertices["pos"].append(pos)

            # 線登録
            indices.append([start + 0, start + 1])
            indices.append([start + 2, start + 3])
            indices.append([start + 0, start + 2])
            indices.append([start + 1, start + 3])

            indices.append([start + 4, start + 5])
            indices.append([start + 6, start + 7])
            indices.append([start + 4, start + 6])
            indices.append([start + 5, start + 7])

            indices.append([start + 0, start + 4])
            indices.append([start + 1, start + 5])
            indices.append([start + 2, start + 6])
            indices.append([start + 3, start + 7])

        # 頂点なしなら終了
        if len(vertices["pos"]) <= 0:
            return

        shader = gpu.shader.from_builtin("3D_UNIFORM_COLOR")

        batch = gpu_extras.batch.batch_for_shader(
            shader,
            "LINES",
            vertices,
            indices=indices
        )

        color = (0.5, 1.0, 1.0, 1.0)

        shader.bind()
        shader.uniform_float("color", color)

        batch.draw(shader)


# =========================================================
# 頂点を伸ばす
# =========================================================
class MYADDON_OT_stretch_vertex(bpy.types.Operator):
    bl_idname = "myaddon.myaddon_ot_stretch_vertex"
    bl_label = "頂点を伸ばす"
    bl_description = "頂点座標を引っ張って伸ばします"
    bl_options = {'REGISTER', 'UNDO'}

    def execute(self, context):

        bpy.data.objects["Cube"].data.vertices[0].co.x += 1.0

        print("頂点を伸ばしました。")

        return {'FINISHED'}


# =========================================================
# ICO球生成
# =========================================================
class MYADDON_OT_create_ico_sphere(bpy.types.Operator):
    bl_idname = "myaddon.myaddon_ot_create_object"
    bl_label = "ICO球生成"
    bl_description = "ICO球を生成します"
    bl_options = {'REGISTER', 'UNDO'}

    def execute(self, context):

        bpy.ops.mesh.primitive_ico_sphere_add()

        print("ICO球を生成しました。")

        return {'FINISHED'}


# =========================================================
# file_name追加
# =========================================================
class MYADDON_OT_add_filename(bpy.types.Operator):
    bl_idname = "myaddon.myaddon_ot_add_filename"
    bl_label = "FileName追加"
    bl_description = "['file_name']カスタムプロパティを追加します"
    bl_options = {'REGISTER', 'UNDO'}

    def execute(self, context):

        context.object["file_name"] = ""

        return {'FINISHED'}


# =========================================================
# collider追加
# =========================================================
class MYADDON_OT_add_collider(bpy.types.Operator):
    bl_idname = "myaddon.myaddon_ot_add_collider"
    bl_label = "Collider追加"
    bl_description = "['collider']カスタムプロパティを追加します"
    bl_options = {'REGISTER', 'UNDO'}

    def execute(self, context):

        context.object["collider"] = "BOX"
        context.object["collider_center"] = mathutils.Vector((0, 0, 0))
        context.object["collider_size"] = mathutils.Vector((2, 2, 2))

        return {'FINISHED'}


# =========================================================
# シーン出力
# =========================================================
class MYADDON_OT_export_scene(
    bpy.types.Operator,
    bpy_extras.io_utils.ExportHelper
):
    bl_idname = "myaddon.myaddon_ot_export_scene"
    bl_label = "シーン出力"
    bl_description = "シーン情報をExportします"

    filename_ext = ".txt"

    def write_and_print(self, file, text):

        print(text)

        file.write(text)
        file.write('\n')

    def parse_scene_recursive(self, file, object, level):

        indent = ""
        for i in range(level):
            indent += "\t"

        self.write_and_print(
            file,
            indent + object.type
        )

        trans, rot, scale = object.matrix_local.decompose()

        rot = rot.to_euler()

        rot.x = math.degrees(rot.x)
        rot.y = math.degrees(rot.y)
        rot.z = math.degrees(rot.z)

        self.write_and_print(
            file,
            indent + "T %f %f %f"
            % (trans.x, trans.y, trans.z)
        )

        self.write_and_print(
            file,
            indent + "R %f %f %f"
            % (rot.x, rot.y, rot.z)
        )

        self.write_and_print(
            file,
            indent + "S %f %f %f"
            % (scale.x, scale.y, scale.z)
        )

        # file_name
        if "file_name" in object:

            self.write_and_print(
                file,
                indent + "N %s"
                % object["file_name"]
            )

        # collider
        if "collider" in object:

            self.write_and_print(
                file,
                indent + "C %s"
                % object["collider"]
            )

            temp_str = indent + "CC %f %f %f"
            temp_str %= (
                object["collider_center"][0],
                object["collider_center"][1],
                object["collider_center"][2]
            )

            self.write_and_print(file, temp_str)

            temp_str = indent + "CS %f %f %f"
            temp_str %= (
                object["collider_size"][0],
                object["collider_size"][1],
                object["collider_size"][2]
            )

            self.write_and_print(file, temp_str)

        self.write_and_print(
            file,
            indent + "END"
        )

        self.write_and_print(file, "")

        for child in object.children:
            self.parse_scene_recursive(
                file,
                child,
                level + 1
            )

    def export(self):

        print("シーン情報をExportします")

        with open(self.filepath, "wt", encoding="utf-8") as file:

            file.write("SCENE\n")

            for object in bpy.context.scene.objects:

                if object.parent:
                    continue

                self.parse_scene_recursive(
                    file,
                    object,
                    0
                )

    def execute(self, context):

        self.export()

        self.report(
            {'INFO'},
            "シーン情報をExportしました"
        )

        return {'FINISHED'}


# =========================================================
# FileNameパネル
# =========================================================
class OBJECT_PT_file_name(bpy.types.Panel):

    bl_idname = "OBJECT_PT_file_name"
    bl_label = "FileName"
    bl_space_type = "PROPERTIES"
    bl_region_type = "WINDOW"
    bl_context = "object"

    def draw(self, context):

        if "file_name" in context.object:
            self.layout.prop(
                context.object,
                '["file_name"]',
                text=self.bl_label
            )
        else:
            self.layout.operator(
                MYADDON_OT_add_filename.bl_idname
            )


# =========================================================
# Colliderパネル
# =========================================================
class OBJECT_PT_collider(bpy.types.Panel):

    bl_idname = "OBJECT_PT_collider"
    bl_label = "Collider"
    bl_space_type = "PROPERTIES"
    bl_region_type = "WINDOW"
    bl_context = "object"

    def draw(self, context):

        if "collider" in context.object:

            self.layout.prop(
                context.object,
                '["collider"]',
                text="Type"
            )

            self.layout.prop(
                context.object,
                '["collider_center"]',
                text="Center"
            )

            self.layout.prop(
                context.object,
                '["collider_size"]',
                text="Size"
            )

        else:

            self.layout.operator(
                MYADDON_OT_add_collider.bl_idname
            )


# =========================================================
# メニュー
# =========================================================
class TOPBAR_MT_my_menu(bpy.types.Menu):

    bl_idname = "TOPBAR_MT_my_menu"
    bl_label = "MyMenu"
    bl_description = "拡張メニュー"

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

    def submenu(self, context):

        self.layout.menu(
            TOPBAR_MT_my_menu.bl_idname
        )


# =========================================================
# クラス登録
# =========================================================
classes = (
    MYADDON_OT_stretch_vertex,
    MYADDON_OT_create_ico_sphere,
    MYADDON_OT_add_filename,
    MYADDON_OT_add_collider,
    MYADDON_OT_export_scene,
    OBJECT_PT_file_name,
    OBJECT_PT_collider,
    TOPBAR_MT_my_menu,
)


# =========================================================
# register
# =========================================================
def register():

    for cls in classes:
        bpy.utils.register_class(cls)

    bpy.types.TOPBAR_MT_editor_menus.append(
        TOPBAR_MT_my_menu.submenu
    )

    DrawCollider.handle = bpy.types.SpaceView3D.draw_handler_add(
        DrawCollider.draw_collider,
        (),
        "WINDOW",
        "POST_VIEW"
    )

    print("レベルエディタが有効化されました。")


# =========================================================
# unregister
# =========================================================
def unregister():

    bpy.types.TOPBAR_MT_editor_menus.remove(
        TOPBAR_MT_my_menu.submenu
    )

    bpy.types.SpaceView3D.draw_handler_remove(
        DrawCollider.handle,
        "WINDOW"
    )

    for cls in classes:
        bpy.utils.unregister_class(cls)

    print("レベルエディタが無効化されました。")


# =========================================================
# テスト実行
# =========================================================
if __name__ == "__main__":
    register()