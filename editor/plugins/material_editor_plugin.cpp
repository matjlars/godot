/*************************************************************************/
/*  material_editor_plugin.cpp                                           */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/*************************************************************************/
/* Copyright (c) 2007-2022 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2022 Godot Engine contributors (cf. AUTHORS.md).   */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/

#include "material_editor_plugin.h"

#include "editor/editor_node.h"
#include "editor/editor_scale.h"
#include "editor/editor_settings.h"
#include "scene/gui/subviewport_container.h"
#include "scene/resources/fog_material.h"
#include "scene/resources/particles_material.h"
#include "scene/resources/sky_material.h"

void MaterialEditor::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_READY: {
			//get_scene()->connect("node_removed",this,"_node_removed");

			if (first_enter) {
				//it's in propertyeditor so.. could be moved around

				light_1_switch->set_normal_texture(get_theme_icon(SNAME("MaterialPreviewLight1"), SNAME("EditorIcons")));
				light_1_switch->set_pressed_texture(get_theme_icon(SNAME("MaterialPreviewLight1Off"), SNAME("EditorIcons")));
				light_2_switch->set_normal_texture(get_theme_icon(SNAME("MaterialPreviewLight2"), SNAME("EditorIcons")));
				light_2_switch->set_pressed_texture(get_theme_icon(SNAME("MaterialPreviewLight2Off"), SNAME("EditorIcons")));

				sphere_switch->set_normal_texture(get_theme_icon(SNAME("MaterialPreviewSphereOff"), SNAME("EditorIcons")));
				sphere_switch->set_pressed_texture(get_theme_icon(SNAME("MaterialPreviewSphere"), SNAME("EditorIcons")));
				box_switch->set_normal_texture(get_theme_icon(SNAME("MaterialPreviewCubeOff"), SNAME("EditorIcons")));
				box_switch->set_pressed_texture(get_theme_icon(SNAME("MaterialPreviewCube"), SNAME("EditorIcons")));

				first_enter = false;
			}
		} break;

		case NOTIFICATION_DRAW: {
			Ref<Texture2D> checkerboard = get_theme_icon(SNAME("Checkerboard"), SNAME("EditorIcons"));
			Size2 size = get_size();

			draw_texture_rect(checkerboard, Rect2(Point2(), size), true);
		} break;
	}
}

void MaterialEditor::edit(Ref<Material> p_material, const Ref<Environment> &p_env) {
	material = p_material;
	camera->set_environment(p_env);
	if (!material.is_null()) {
		Shader::Mode mode = p_material->get_shader_mode();
		switch (mode) {
			case Shader::MODE_CANVAS_ITEM:
				layout_3d->hide();
				layout_2d->show();
				vc->hide();
				rect_instance->set_material(material);
				break;
			case Shader::MODE_SPATIAL:
				layout_2d->hide();
				layout_3d->show();
				vc->show();
				sphere_instance->set_material_override(material);
				box_instance->set_material_override(material);
				break;
			default:
				break;
		}
	} else {
		hide();
	}
}

void MaterialEditor::_button_pressed(Node *p_button) {
	if (p_button == light_1_switch) {
		light1->set_visible(!light_1_switch->is_pressed());
	}

	if (p_button == light_2_switch) {
		light2->set_visible(!light_2_switch->is_pressed());
	}

	if (p_button == box_switch) {
		box_instance->show();
		sphere_instance->hide();
		box_switch->set_pressed(true);
		sphere_switch->set_pressed(false);
		EditorSettings::get_singleton()->set_project_metadata("inspector_options", "material_preview_on_sphere", false);
	}

	if (p_button == sphere_switch) {
		box_instance->hide();
		sphere_instance->show();
		box_switch->set_pressed(false);
		sphere_switch->set_pressed(true);
		EditorSettings::get_singleton()->set_project_metadata("inspector_options", "material_preview_on_sphere", true);
	}
}

void MaterialEditor::_bind_methods() {
}

MaterialEditor::MaterialEditor() {
	// canvas item

	layout_2d = memnew(HBoxContainer);
	layout_2d->set_alignment(BoxContainer::ALIGNMENT_CENTER);
	add_child(layout_2d);
	layout_2d->set_anchors_and_offsets_preset(PRESET_FULL_RECT);

	rect_instance = memnew(ColorRect);
	layout_2d->add_child(rect_instance);
	rect_instance->set_custom_minimum_size(Size2(150, 150) * EDSCALE);

	layout_2d->set_visible(false);

	// spatial

	vc = memnew(SubViewportContainer);
	vc->set_stretch(true);
	add_child(vc);
	vc->set_anchors_and_offsets_preset(PRESET_FULL_RECT);
	viewport = memnew(SubViewport);
	Ref<World3D> world_3d;
	world_3d.instantiate();
	viewport->set_world_3d(world_3d); //use own world
	vc->add_child(viewport);
	viewport->set_disable_input(true);
	viewport->set_transparent_background(true);
	viewport->set_msaa(Viewport::MSAA_4X);

	camera = memnew(Camera3D);
	camera->set_transform(Transform3D(Basis(), Vector3(0, 0, 3)));
	// Use low field of view so the sphere/box is fully encompassed within the preview,
	// without much distortion.
	camera->set_perspective(20, 0.1, 10);
	camera->make_current();
	viewport->add_child(camera);

	light1 = memnew(DirectionalLight3D);
	light1->set_transform(Transform3D().looking_at(Vector3(-1, -1, -1), Vector3(0, 1, 0)));
	viewport->add_child(light1);

	light2 = memnew(DirectionalLight3D);
	light2->set_transform(Transform3D().looking_at(Vector3(0, 1, 0), Vector3(0, 0, 1)));
	light2->set_color(Color(0.7, 0.7, 0.7));
	viewport->add_child(light2);

	sphere_instance = memnew(MeshInstance3D);
	viewport->add_child(sphere_instance);

	box_instance = memnew(MeshInstance3D);
	viewport->add_child(box_instance);

	Transform3D box_xform;
	box_xform.basis.rotate(Vector3(1, 0, 0), Math::deg2rad(25.0));
	box_xform.basis = box_xform.basis * Basis().rotated(Vector3(0, 1, 0), Math::deg2rad(-25.0));
	box_xform.basis.scale(Vector3(0.7, 0.7, 0.7));
	box_xform.origin.y = 0.05;
	box_instance->set_transform(box_xform);

	sphere_mesh.instantiate();
	sphere_instance->set_mesh(sphere_mesh);
	box_mesh.instantiate();
	box_instance->set_mesh(box_mesh);

	set_custom_minimum_size(Size2(1, 150) * EDSCALE);

	layout_3d = memnew(HBoxContainer);
	add_child(layout_3d);
	layout_3d->set_anchors_and_offsets_preset(Control::PRESET_FULL_RECT, Control::PRESET_MODE_MINSIZE, 2);

	VBoxContainer *vb_shape = memnew(VBoxContainer);
	layout_3d->add_child(vb_shape);

	sphere_switch = memnew(TextureButton);
	sphere_switch->set_toggle_mode(true);
	sphere_switch->set_pressed(true);
	vb_shape->add_child(sphere_switch);
	sphere_switch->connect("pressed", callable_mp(this, &MaterialEditor::_button_pressed).bind(sphere_switch));

	box_switch = memnew(TextureButton);
	box_switch->set_toggle_mode(true);
	box_switch->set_pressed(false);
	vb_shape->add_child(box_switch);
	box_switch->connect("pressed", callable_mp(this, &MaterialEditor::_button_pressed).bind(box_switch));

	layout_3d->add_spacer();

	VBoxContainer *vb_light = memnew(VBoxContainer);
	layout_3d->add_child(vb_light);

	light_1_switch = memnew(TextureButton);
	light_1_switch->set_toggle_mode(true);
	vb_light->add_child(light_1_switch);
	light_1_switch->connect("pressed", callable_mp(this, &MaterialEditor::_button_pressed).bind(light_1_switch));

	light_2_switch = memnew(TextureButton);
	light_2_switch->set_toggle_mode(true);
	vb_light->add_child(light_2_switch);
	light_2_switch->connect("pressed", callable_mp(this, &MaterialEditor::_button_pressed).bind(light_2_switch));

	first_enter = true;

	if (EditorSettings::get_singleton()->get_project_metadata("inspector_options", "material_preview_on_sphere", true)) {
		box_instance->hide();
	} else {
		box_instance->show();
		sphere_instance->hide();
		box_switch->set_pressed(true);
		sphere_switch->set_pressed(false);
	}
}

///////////////////////

bool EditorInspectorPluginMaterial::can_handle(Object *p_object) {
	Material *material = Object::cast_to<Material>(p_object);
	if (!material) {
		return false;
	}
	Shader::Mode mode = material->get_shader_mode();
	return mode == Shader::MODE_SPATIAL || mode == Shader::MODE_CANVAS_ITEM;
}

void EditorInspectorPluginMaterial::parse_begin(Object *p_object) {
	Material *material = Object::cast_to<Material>(p_object);
	if (!material) {
		return;
	}
	Ref<Material> m(material);

	MaterialEditor *editor = memnew(MaterialEditor);
	editor->edit(m, env);
	add_custom_control(editor);
}

void EditorInspectorPluginMaterial::_undo_redo_inspector_callback(Object *p_undo_redo, Object *p_edited, String p_property, Variant p_new_value) {
	UndoRedo *undo_redo = Object::cast_to<UndoRedo>(p_undo_redo);
	if (!undo_redo) {
		return;
	}

	// For BaseMaterial3D, if a roughness or metallic textures is being assigned to an empty slot,
	// set the respective metallic or roughness factor to 1.0 as a convenience feature
	BaseMaterial3D *base_material = Object::cast_to<StandardMaterial3D>(p_edited);
	if (base_material) {
		Texture2D *texture = Object::cast_to<Texture2D>(p_new_value);
		if (texture) {
			if (p_property == "roughness_texture") {
				if (base_material->get_texture(StandardMaterial3D::TEXTURE_ROUGHNESS).is_null()) {
					undo_redo->add_do_property(p_edited, "roughness", 1.0);

					bool valid = false;
					Variant value = p_edited->get("roughness", &valid);
					if (valid) {
						undo_redo->add_undo_property(p_edited, "roughness", value);
					}
				}
			} else if (p_property == "metallic_texture") {
				if (base_material->get_texture(StandardMaterial3D::TEXTURE_METALLIC).is_null()) {
					undo_redo->add_do_property(p_edited, "metallic", 1.0);

					bool valid = false;
					Variant value = p_edited->get("metallic", &valid);
					if (valid) {
						undo_redo->add_undo_property(p_edited, "metallic", value);
					}
				}
			}
		}
	}
}

EditorInspectorPluginMaterial::EditorInspectorPluginMaterial() {
	env.instantiate();
	Ref<Sky> sky = memnew(Sky());
	env->set_sky(sky);
	env->set_background(Environment::BG_COLOR);
	env->set_ambient_source(Environment::AMBIENT_SOURCE_SKY);
	env->set_reflection_source(Environment::REFLECTION_SOURCE_SKY);

	EditorNode::get_singleton()->get_editor_data().add_undo_redo_inspector_hook_callback(callable_mp(this, &EditorInspectorPluginMaterial::_undo_redo_inspector_callback));
}

MaterialEditorPlugin::MaterialEditorPlugin() {
	Ref<EditorInspectorPluginMaterial> plugin;
	plugin.instantiate();
	add_inspector_plugin(plugin);
}

String StandardMaterial3DConversionPlugin::converts_to() const {
	return "ShaderMaterial";
}

bool StandardMaterial3DConversionPlugin::handles(const Ref<Resource> &p_resource) const {
	Ref<StandardMaterial3D> mat = p_resource;
	return mat.is_valid();
}

Ref<Resource> StandardMaterial3DConversionPlugin::convert(const Ref<Resource> &p_resource) const {
	Ref<StandardMaterial3D> mat = p_resource;
	ERR_FAIL_COND_V(!mat.is_valid(), Ref<Resource>());

	Ref<ShaderMaterial> smat;
	smat.instantiate();

	Ref<Shader> shader;
	shader.instantiate();

	String code = RS::get_singleton()->shader_get_code(mat->get_shader_rid());

	shader->set_code(code);

	smat->set_shader(shader);

	List<PropertyInfo> params;
	RS::get_singleton()->shader_get_param_list(mat->get_shader_rid(), &params);

	for (const PropertyInfo &E : params) {
		// Texture parameter has to be treated specially since StandardMaterial3D saved it
		// as RID but ShaderMaterial needs Texture itself
		Ref<Texture2D> texture = mat->get_texture_by_name(E.name);
		if (texture.is_valid()) {
			smat->set_shader_param(E.name, texture);
		} else {
			Variant value = RS::get_singleton()->material_get_param(mat->get_rid(), E.name);
			smat->set_shader_param(E.name, value);
		}
	}

	smat->set_render_priority(mat->get_render_priority());
	smat->set_local_to_scene(mat->is_local_to_scene());
	smat->set_name(mat->get_name());
	return smat;
}

String ORMMaterial3DConversionPlugin::converts_to() const {
	return "ShaderMaterial";
}

bool ORMMaterial3DConversionPlugin::handles(const Ref<Resource> &p_resource) const {
	Ref<ORMMaterial3D> mat = p_resource;
	return mat.is_valid();
}

Ref<Resource> ORMMaterial3DConversionPlugin::convert(const Ref<Resource> &p_resource) const {
	Ref<ORMMaterial3D> mat = p_resource;
	ERR_FAIL_COND_V(!mat.is_valid(), Ref<Resource>());

	Ref<ShaderMaterial> smat;
	smat.instantiate();

	Ref<Shader> shader;
	shader.instantiate();

	String code = RS::get_singleton()->shader_get_code(mat->get_shader_rid());

	shader->set_code(code);

	smat->set_shader(shader);

	List<PropertyInfo> params;
	RS::get_singleton()->shader_get_param_list(mat->get_shader_rid(), &params);

	for (const PropertyInfo &E : params) {
		// Texture parameter has to be treated specially since ORMMaterial3D saved it
		// as RID but ShaderMaterial needs Texture itself
		Ref<Texture2D> texture = mat->get_texture_by_name(E.name);
		if (texture.is_valid()) {
			smat->set_shader_param(E.name, texture);
		} else {
			Variant value = RS::get_singleton()->material_get_param(mat->get_rid(), E.name);
			smat->set_shader_param(E.name, value);
		}
	}

	smat->set_render_priority(mat->get_render_priority());
	smat->set_local_to_scene(mat->is_local_to_scene());
	smat->set_name(mat->get_name());
	return smat;
}

String ParticlesMaterialConversionPlugin::converts_to() const {
	return "ShaderMaterial";
}

bool ParticlesMaterialConversionPlugin::handles(const Ref<Resource> &p_resource) const {
	Ref<ParticlesMaterial> mat = p_resource;
	return mat.is_valid();
}

Ref<Resource> ParticlesMaterialConversionPlugin::convert(const Ref<Resource> &p_resource) const {
	Ref<ParticlesMaterial> mat = p_resource;
	ERR_FAIL_COND_V(!mat.is_valid(), Ref<Resource>());

	Ref<ShaderMaterial> smat;
	smat.instantiate();

	Ref<Shader> shader;
	shader.instantiate();

	String code = RS::get_singleton()->shader_get_code(mat->get_shader_rid());

	shader->set_code(code);

	smat->set_shader(shader);

	List<PropertyInfo> params;
	RS::get_singleton()->shader_get_param_list(mat->get_shader_rid(), &params);

	for (const PropertyInfo &E : params) {
		Variant value = RS::get_singleton()->material_get_param(mat->get_rid(), E.name);
		smat->set_shader_param(E.name, value);
	}

	smat->set_render_priority(mat->get_render_priority());
	smat->set_local_to_scene(mat->is_local_to_scene());
	smat->set_name(mat->get_name());
	return smat;
}

String CanvasItemMaterialConversionPlugin::converts_to() const {
	return "ShaderMaterial";
}

bool CanvasItemMaterialConversionPlugin::handles(const Ref<Resource> &p_resource) const {
	Ref<CanvasItemMaterial> mat = p_resource;
	return mat.is_valid();
}

Ref<Resource> CanvasItemMaterialConversionPlugin::convert(const Ref<Resource> &p_resource) const {
	Ref<CanvasItemMaterial> mat = p_resource;
	ERR_FAIL_COND_V(!mat.is_valid(), Ref<Resource>());

	Ref<ShaderMaterial> smat;
	smat.instantiate();

	Ref<Shader> shader;
	shader.instantiate();

	String code = RS::get_singleton()->shader_get_code(mat->get_shader_rid());

	shader->set_code(code);

	smat->set_shader(shader);

	List<PropertyInfo> params;
	RS::get_singleton()->shader_get_param_list(mat->get_shader_rid(), &params);

	for (const PropertyInfo &E : params) {
		Variant value = RS::get_singleton()->material_get_param(mat->get_rid(), E.name);
		smat->set_shader_param(E.name, value);
	}

	smat->set_render_priority(mat->get_render_priority());
	smat->set_local_to_scene(mat->is_local_to_scene());
	smat->set_name(mat->get_name());
	return smat;
}

String ProceduralSkyMaterialConversionPlugin::converts_to() const {
	return "ShaderMaterial";
}

bool ProceduralSkyMaterialConversionPlugin::handles(const Ref<Resource> &p_resource) const {
	Ref<ProceduralSkyMaterial> mat = p_resource;
	return mat.is_valid();
}

Ref<Resource> ProceduralSkyMaterialConversionPlugin::convert(const Ref<Resource> &p_resource) const {
	Ref<ProceduralSkyMaterial> mat = p_resource;
	ERR_FAIL_COND_V(!mat.is_valid(), Ref<Resource>());

	Ref<ShaderMaterial> smat;
	smat.instantiate();

	Ref<Shader> shader;
	shader.instantiate();

	String code = RS::get_singleton()->shader_get_code(mat->get_shader_rid());

	shader->set_code(code);

	smat->set_shader(shader);

	List<PropertyInfo> params;
	RS::get_singleton()->shader_get_param_list(mat->get_shader_rid(), &params);

	for (const PropertyInfo &E : params) {
		Variant value = RS::get_singleton()->material_get_param(mat->get_rid(), E.name);
		smat->set_shader_param(E.name, value);
	}

	smat->set_render_priority(mat->get_render_priority());
	smat->set_local_to_scene(mat->is_local_to_scene());
	smat->set_name(mat->get_name());
	return smat;
}

String PanoramaSkyMaterialConversionPlugin::converts_to() const {
	return "ShaderMaterial";
}

bool PanoramaSkyMaterialConversionPlugin::handles(const Ref<Resource> &p_resource) const {
	Ref<PanoramaSkyMaterial> mat = p_resource;
	return mat.is_valid();
}

Ref<Resource> PanoramaSkyMaterialConversionPlugin::convert(const Ref<Resource> &p_resource) const {
	Ref<PanoramaSkyMaterial> mat = p_resource;
	ERR_FAIL_COND_V(!mat.is_valid(), Ref<Resource>());

	Ref<ShaderMaterial> smat;
	smat.instantiate();

	Ref<Shader> shader;
	shader.instantiate();

	String code = RS::get_singleton()->shader_get_code(mat->get_shader_rid());

	shader->set_code(code);

	smat->set_shader(shader);

	List<PropertyInfo> params;
	RS::get_singleton()->shader_get_param_list(mat->get_shader_rid(), &params);

	for (const PropertyInfo &E : params) {
		Variant value = RS::get_singleton()->material_get_param(mat->get_rid(), E.name);
		smat->set_shader_param(E.name, value);
	}

	smat->set_render_priority(mat->get_render_priority());
	smat->set_local_to_scene(mat->is_local_to_scene());
	smat->set_name(mat->get_name());
	return smat;
}

String PhysicalSkyMaterialConversionPlugin::converts_to() const {
	return "ShaderMaterial";
}

bool PhysicalSkyMaterialConversionPlugin::handles(const Ref<Resource> &p_resource) const {
	Ref<PhysicalSkyMaterial> mat = p_resource;
	return mat.is_valid();
}

Ref<Resource> PhysicalSkyMaterialConversionPlugin::convert(const Ref<Resource> &p_resource) const {
	Ref<PhysicalSkyMaterial> mat = p_resource;
	ERR_FAIL_COND_V(!mat.is_valid(), Ref<Resource>());

	Ref<ShaderMaterial> smat;
	smat.instantiate();

	Ref<Shader> shader;
	shader.instantiate();

	String code = RS::get_singleton()->shader_get_code(mat->get_shader_rid());

	shader->set_code(code);

	smat->set_shader(shader);

	List<PropertyInfo> params;
	RS::get_singleton()->shader_get_param_list(mat->get_shader_rid(), &params);

	for (const PropertyInfo &E : params) {
		Variant value = RS::get_singleton()->material_get_param(mat->get_rid(), E.name);
		smat->set_shader_param(E.name, value);
	}

	smat->set_render_priority(mat->get_render_priority());
	smat->set_local_to_scene(mat->is_local_to_scene());
	smat->set_name(mat->get_name());
	return smat;
}

String FogMaterialConversionPlugin::converts_to() const {
	return "ShaderMaterial";
}

bool FogMaterialConversionPlugin::handles(const Ref<Resource> &p_resource) const {
	Ref<FogMaterial> mat = p_resource;
	return mat.is_valid();
}

Ref<Resource> FogMaterialConversionPlugin::convert(const Ref<Resource> &p_resource) const {
	Ref<FogMaterial> mat = p_resource;
	ERR_FAIL_COND_V(!mat.is_valid(), Ref<Resource>());

	Ref<ShaderMaterial> smat;
	smat.instantiate();

	Ref<Shader> shader;
	shader.instantiate();

	String code = RS::get_singleton()->shader_get_code(mat->get_shader_rid());

	shader->set_code(code);

	smat->set_shader(shader);

	List<PropertyInfo> params;
	RS::get_singleton()->shader_get_param_list(mat->get_shader_rid(), &params);

	for (const PropertyInfo &E : params) {
		Variant value = RS::get_singleton()->material_get_param(mat->get_rid(), E.name);
		smat->set_shader_param(E.name, value);
	}

	smat->set_render_priority(mat->get_render_priority());
	return smat;
}
