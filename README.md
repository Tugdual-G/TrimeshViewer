## Viewer for triangular mesh .ply files
__A small project to learn c++.__
The program can parse .ply files for triangular mesh,
and plot the data.

__Capabilities:__
- flat shading and smooth shading
- Use colormaps to show scalar quantities
- compute curvature & normal
- zoom and rotate with the mouse

## Requirements
- opengl 4.6
- glfw

## Compilation

        make -C sources

## Usage

        ./meshviewer meshes/deform.ply

__Code example :__

```
int main() {

  PlyFile file("meshes/deformHQ.ply");

  // Retrieving the vertices normals from the file.
  std::vector<PropertyName> normal_property_names = {
      PropertyName::nx, PropertyName::ny, PropertyName::nz};
  std::vector<double> normals;
  file.get_subelement_data<double>("vertices", normal_property_names, normals);


  Mesh mesh(file.vertices, file.faces);

  // Computing the curvature to define the colors of the rendered object.
  std::vector<double> k;
  mesh.set_one_ring(); // the one ring is required to compute the curvature
  mesh.set_mean_curvature();

  mesh.scalar_mean_curvature(k);

  std::vector<double> colors;
  get_interpolated_colors(k, colors, INFERNO);

  MeshRender render(500, 500, mesh.vertices, mesh.faces, normals);

  render.add_vertex_colors(colors);
  render.render_loop(NULL, NULL);
  render.render_finalize();
  return 0;
}
```
