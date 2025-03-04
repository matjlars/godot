/*************************************************************************/
/*  material_editor_plugin.h                                             */
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

#ifndef MATERIAL_EDITOR_PLUGIN_H
#define MATERIAL_EDITOR_PLUGIN_H

#include "editor/editor_plugin.h"
#include "editor/plugins/editor_resource_conversion_plugin.h"
#include "scene/3d/camera_3d.h"
#include "scene/3d/light_3d.h"
#include "scene/3d/mesh_instance_3d.h"
#include "scene/gui/color_rect.h"
#include "scene/resources/material.h"
#include "scene/resources/primitive_meshes.h"

class SubViewportContainer;

class MaterialEditor : public Control {
	GDCLASS(MaterialEditor, Control);

	HBoxContainer *layout_2d = nullptr;
	ColorRect *rect_instance = nullptr;

	SubViewportContainer *vc = nullptr;
	SubViewport *viewport = nullptr;
	MeshInstance3D *sphere_instance = nullptr;
	MeshInstance3D *box_instance = nullptr;
	DirectionalLight3D *light1 = nullptr;
	DirectionalLight3D *light2 = nullptr;
	Camera3D *camera = nullptr;

	Ref<SphereMesh> sphere_mesh;
	Ref<BoxMesh> box_mesh;

	HBoxContainer *layout_3d = nullptr;

	TextureButton *sphere_switch = nullptr;
	TextureButton *box_switch = nullptr;

	TextureButton *light_1_switch = nullptr;
	TextureButton *light_2_switch = nullptr;

	Ref<Material> material;

	void _button_pressed(Node *p_button);
	bool first_enter;

protected:
	void _notification(int p_what);

	static void _bind_methods();

public:
	void edit(Ref<Material> p_material, const Ref<Environment> &p_env);
	MaterialEditor();
};

class EditorInspectorPluginMaterial : public EditorInspectorPlugin {
	GDCLASS(EditorInspectorPluginMaterial, EditorInspectorPlugin);
	Ref<Environment> env;

public:
	virtual bool can_handle(Object *p_object) override;
	virtual void parse_begin(Object *p_object) override;

	void _undo_redo_inspector_callback(Object *p_undo_redo, Object *p_edited, String p_property, Variant p_new_value);

	EditorInspectorPluginMaterial();
};

class MaterialEditorPlugin : public EditorPlugin {
	GDCLASS(MaterialEditorPlugin, EditorPlugin);

public:
	virtual String get_name() const override { return "Material"; }

	MaterialEditorPlugin();
};

class StandardMaterial3DConversionPlugin : public EditorResourceConversionPlugin {
	GDCLASS(StandardMaterial3DConversionPlugin, EditorResourceConversionPlugin);

public:
	virtual String converts_to() const override;
	virtual bool handles(const Ref<Resource> &p_resource) const override;
	virtual Ref<Resource> convert(const Ref<Resource> &p_resource) const override;
};

class ORMMaterial3DConversionPlugin : public EditorResourceConversionPlugin {
	GDCLASS(ORMMaterial3DConversionPlugin, EditorResourceConversionPlugin);

public:
	virtual String converts_to() const override;
	virtual bool handles(const Ref<Resource> &p_resource) const override;
	virtual Ref<Resource> convert(const Ref<Resource> &p_resource) const override;
};

class ParticlesMaterialConversionPlugin : public EditorResourceConversionPlugin {
	GDCLASS(ParticlesMaterialConversionPlugin, EditorResourceConversionPlugin);

public:
	virtual String converts_to() const override;
	virtual bool handles(const Ref<Resource> &p_resource) const override;
	virtual Ref<Resource> convert(const Ref<Resource> &p_resource) const override;
};

class CanvasItemMaterialConversionPlugin : public EditorResourceConversionPlugin {
	GDCLASS(CanvasItemMaterialConversionPlugin, EditorResourceConversionPlugin);

public:
	virtual String converts_to() const override;
	virtual bool handles(const Ref<Resource> &p_resource) const override;
	virtual Ref<Resource> convert(const Ref<Resource> &p_resource) const override;
};

class ProceduralSkyMaterialConversionPlugin : public EditorResourceConversionPlugin {
	GDCLASS(ProceduralSkyMaterialConversionPlugin, EditorResourceConversionPlugin);

public:
	virtual String converts_to() const override;
	virtual bool handles(const Ref<Resource> &p_resource) const override;
	virtual Ref<Resource> convert(const Ref<Resource> &p_resource) const override;
};

class PanoramaSkyMaterialConversionPlugin : public EditorResourceConversionPlugin {
	GDCLASS(PanoramaSkyMaterialConversionPlugin, EditorResourceConversionPlugin);

public:
	virtual String converts_to() const override;
	virtual bool handles(const Ref<Resource> &p_resource) const override;
	virtual Ref<Resource> convert(const Ref<Resource> &p_resource) const override;
};

class PhysicalSkyMaterialConversionPlugin : public EditorResourceConversionPlugin {
	GDCLASS(PhysicalSkyMaterialConversionPlugin, EditorResourceConversionPlugin);

public:
	virtual String converts_to() const override;
	virtual bool handles(const Ref<Resource> &p_resource) const override;
	virtual Ref<Resource> convert(const Ref<Resource> &p_resource) const override;
};

class FogMaterialConversionPlugin : public EditorResourceConversionPlugin {
	GDCLASS(FogMaterialConversionPlugin, EditorResourceConversionPlugin);

public:
	virtual String converts_to() const override;
	virtual bool handles(const Ref<Resource> &p_resource) const override;
	virtual Ref<Resource> convert(const Ref<Resource> &p_resource) const override;
};

#endif // MATERIAL_EDITOR_PLUGIN_H
