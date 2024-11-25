#ifndef __GPS_TOBD09_H__
#define __GPS_TOBD09_H__

int out_of_china(double lon, double lat);
double transform_latitude(double lon, double lat);
double transform_longitude(double lon, double lat);
void wgs84_to_gcj02(double lon, double lat, double *output);
void gcj02_to_wgs84(double lon, double lat, double *output);
void gcj02_to_bd09(double lon, double lat, double *output);
void bd09_to_gcj02(double lon, double lat, double *output);
void wgs84_to_bd09(double lon, double lat, double *output);
void bd09_to_wgs84(double lon, double lat, double *output);


#endif
