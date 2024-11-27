#include "math.h"
#include "stdlib.h"
#include "stdio.h"
#include "main.h"
#include "gps_Tobd09.h"

#define PI M_PI
#define X_PI (PI * 3000.0 / 180.0)
#define A 6378245.0
#define EE 0.00669342162296594323

/**
 * 判断给定的经纬度是否在中国境内
 *
 * @param lon 经度值
 * @param lat 纬度值
 * @return 如果在中国境内返回0，否则返回1
 */
int out_of_china(double lon, double lat) {
    if (lon < 72.004 || lon > 137.8347) {
        return 1;
    }
    if (lat < 0.8293 || lat > 55.8271) {
        return 1;
    }
    return 0;
}


double transform_latitude(double lon, double lat) {
    double ret = -100.0 + 2.0 * lon + 3.0 * lat + 0.2 * lat * lat + 0.1 * lon * lat + 0.2 * sqrt(fabs(lon));
    ret += (20.0 * sin(6.0 * lon * PI) + 20.0 * sin(2.0 * lon * PI)) * 2.0 / 3.0;
    ret += (20.0 * sin(lat * PI) + 40.0 * sin(lat / 3.0 * PI)) * 2.0 / 3.0;
    ret += (160.0 * sin(lat / 12.0 * PI) + 320 * sin(lat * PI / 30.0)) * 2.0 / 3.0;
    return ret;
}

double transform_longitude(double lon, double lat) {
    double ret = 300.0 + lon + 2.0 * lat + 0.1 * lon * lon + 0.1 * lon * lat + 0.1 * sqrt(fabs(lon));
    ret += (20.0 * sin(6.0 * lon * PI) + 20.0 * sin(2.0 * lon * PI)) * 2.0 / 3.0;
    ret += (20.0 * sin(lon * PI) + 40.0 * sin(lon / 3.0 * PI)) * 2.0 / 3.0;
    ret += (150.0 * sin(lon / 12.0 * PI) + 300.0 * sin(lon / 30.0 * PI)) * 2.0 / 3.0;
    return ret;
}

/**
 * 将WGS84坐标转换为GCJ02坐标
 *
 * @param lon 经度值
 * @param lat 纬度值
 * @param output 输出数组，长度为2
 */
void wgs84_to_gcj02(double lon, double lat, double *output) {
    if (out_of_china(lon, lat) == 0) {
        double d_lat = transform_latitude(lon - 105.0, lat - 35.0);
        double d_lon = transform_longitude(lon - 105.0, lat - 35.0);
        double rad_lat = lat / 180.0 * PI;
        double magic = sin(rad_lat);
        magic = 1 - EE * magic * magic;
        double sqrt_magic = sqrt(magic);
        d_lat = (d_lat * 180.0) / ((A * (1 - EE)) / (magic * sqrt_magic) * PI);
        d_lon = (d_lon * 180.0) / (A / sqrt_magic * cos(rad_lat) * PI);
        output[0] = lon + d_lon;
        output[1] = lat + d_lat;
    } else {
        output[0] = lon;
        output[1] = lat;
    }
}

/**
 * 将GCJ02坐标转换为WGS84坐标
 *
 * @param lon 经度值
 * @param lat 纬度值
 * @param output 输出数组，长度为2
 */
void gcj02_to_wgs84(double lon, double lat, double *output) {
    if (out_of_china(lon, lat) == 0) {
        double d_lat = transform_latitude(lon - 105.0, lat - 35.0);
        double d_lon = transform_longitude(lon - 105.0, lat - 35.0);
        double rad_lat = lat / 180.0 * PI;
        double magic = sin(rad_lat);
        magic = 1 - EE * magic * magic;
        double sqrt_magic = sqrt(magic);
        d_lat = (d_lat * 180.0) / ((A * (1 - EE)) / (magic * sqrt_magic) * PI);
        d_lon = (d_lon * 180.0) / (A / sqrt_magic * cos(rad_lat) * PI);
        double mg_lat = lat + d_lat;
        double mg_lon = lon + d_lon;
        output[0] = lon * 2 - mg_lon;
        output[1] = lat * 2 - mg_lat;
    } else {
        output[0] = lon;
        output[1] = lat;
    }
}

/**
 * 将GCJ02坐标转换为BD09坐标
 *
 * @param lon 经度值
 * @param lat 纬度值
 * @param output 输出数组，长度为2
 */
void gcj02_to_bd09(double lon, double lat, double *output) {
    double x = lon, y = lat;
    double z = sqrt(x * x + y * y) + 0.00002 * sin(y * X_PI);
    double theta = atan2(y, x) + 0.000003 * cos(x * X_PI);
    output[0] = z * cos(theta) + 0.0065;
    output[1] = z * sin(theta) + 0.006;
}

/**
 * 将BD09坐标转换为GCJ02坐标
 *
 * @param lon 经度值
 * @param lat 纬度值
 * @param output 输出数组，长度为2
 */
void bd09_to_gcj02(double lon, double lat, double *output) {
    double x = lon - 0.0065, y = lat - 0.006;
    double z = sqrt(x * x + y * y) - 0.00002 * sin(y * X_PI);
    double theta = atan2(y, x) - 0.000003 * cos(x * X_PI);
    output[0] = z * cos(theta);
    output[1] = z * sin(theta);
}


/**
 * 将WGS84坐标转换为BD09坐标
 *
 * @param lon 经度值
 * @param lat 纬度值
 * @param output 输出数组，长度为2
 */
void wgs84_to_bd09(double lon, double lat, double *output) {
    double gcj02[2];
    wgs84_to_gcj02(lon, lat, gcj02);
    gcj02_to_bd09(gcj02[0], gcj02[1], output);
}

/**
 * 将BD09坐标转换为WGS84坐标
 *
 * @param lon 经度值
 * @param lat 纬度值
 * @param output 输出数组，长度为2
 */
void bd09_to_wgs84(double lon, double lat, double *output) {
    double gcj02[2];
    bd09_to_gcj02(lon, lat, gcj02);
    gcj02_to_wgs84(gcj02[0], gcj02[1], output);
}


