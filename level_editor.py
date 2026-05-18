import bpy
import math

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
class MYADDON_OT_export_scene(bpy.types.Operator):
    bl_idname = "myaddon.myaddon_ot_export_scene"
    bl_label = "シーン出力"
    bl_description = "シーン情報をExportします"
    bl_options = {'REGISTER', 'UNDO'}

    def write_and_print(self, file, text):
        print(text)
        file.write(text)
        file.write("\n")

    def parse_scene_recursive(self, file, obj, level):
        indent = ''
        for i in range(level):
            indent += '\t'

        self.write_and_print(file, indent + obj.type)

        trans, rot, scale = obj.matrix_local.decompose()
        rot = rot.to_euler()

        rot.x = math.degrees(rot.x)
        rot.y = math.degrees(rot.y)
        rot.z = math.degrees(rot.z)

        self.write_and_print(file, indent + "T %f %f %f" % (trans.x, trans.y, trans.z))
        self.write_and_print(file, indent + "R %f %f %f" % (rot.x, rot.y, rot.z))
        self.write_and_print(file, indent + "S %f %f %f" % (scale.x, scale.y, scale.z))

        if "file_name" in obj:
            self.write_and_print(file, indent + "N %s" % obj["file_name"])

        self.write_and_print(file, indent + "END")
        self.write_and_print(file, "")

        for child in obj.children:
            self.parse_scene_recursive(file, child, level + 1)

    def execute(self, context):
        print("シーン情報をExportします")

        filepath = bpy.data.filepath
        if filepath == "":
            self.report({'WARNING'}, "先にBlenderファイルを保存してください")
            return {'CANCELLED'}

        export_filepath = filepath.replace(".blend", ".txt")

        with open(export_filepath, "w", encoding="utf-8") as file:
            for obj in bpy.context.scene.objects:
                if obj.parent:
                    continue

                self.parse_scene_recursive(file, obj, 0)

        print("シーン情報をExportしました")
        self.report({'INFO'}, "シーン情報をExportしました")
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

    print("レベルエディタが有効化されました。")


# Add-On無効化時コールバック
def unregister():
    bpy.types.TOPBAR_MT_editor_menus.remove(TOPBAR_MT_my_menu.submenu)

    for cls in reversed(classes):
        bpy.utils.unregister_class(cls)

    print("レベルエディタが無効化されました。")


# テスト実行用コード
if __name__ == "__main__":
    register()