# Scene debugging checklist

For a rendering regression, start with the smallest scene that still reproduces the problem.

- Verify the camera and image dimensions.
- Test one primitive before adding additional geometry.
- Check surface normals and intersection distances.
- Add lights incrementally when debugging shading.
- Compare the rendered output against a known-good scene after each change.

Keeping the scene minimal makes geometry, material, and camera bugs much easier to distinguish.