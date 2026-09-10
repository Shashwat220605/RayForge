# Rendering notes

## Numerical stability

Ray intersections and floating-point comparisons should allow for small numerical error. Avoid exact equality checks for values produced by geometric calculations.

## Debugging renders

When a scene changes unexpectedly, reduce the scene to one camera, one light, and a small number of primitives. This makes intersection and shading problems easier to isolate before restoring the full scene.

## Performance

Keep correctness checks and profiling separate. A rendering optimization should be validated against an image or scene where the relevant behavior is observable before comparing render time.