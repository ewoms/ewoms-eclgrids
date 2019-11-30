/*
 * Jostein R. Natvig <Jostein.R.Natvig at sintef.no>
 */
#ifndef MRST_GEOMETRY_H
#define MRST_GEOMETRY_H

void compute_face_geometry(int ndims, double *coords, int nfaces,
                           int *nodepos, int *facenodes,
                           double *fnormals, double *fcentroids,
                           double *fareas);
void compute_cell_geometry(int ndims, double *coords,
                           int *nodepos, int *facenodes, int *neighbours,
                           double *fnormals,
                           double *fcentroids, int ncells,
                           int *facepos, int *cellfaces,
                           double *ccentroids, double *cvolumes);

#endif /* MRST_GEOMETRY_H */
