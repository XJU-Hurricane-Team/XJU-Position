/**
 * @file    position_calc.c
 * @author  Deadline039
 * @brief   计算位置
 * @version 1.1
 * @date    2024-09-30
 */

#include "includes.h"

#include <float.h>
#include <math.h>

#include "arm_math.h"

const static float32_t enc_diameter[2] = {51.009105f, 50.475733f};
const static float32_t enc_cali_cos[2] = {1.0000000f, 0.9999656f};

/* x坐标 */
float g_pos_x = 0.0f;
/* y坐标 */
float g_pos_y = 0.0f;
/* yaw角度 */
float g_pos_yaw = 0.0f;
/* yaw角度偏移 */
float g_pos_yaw_offset = 0.0f;

/**
 * @brief 获取传感器得值计算位置
 *
 * @note 计算完毕后后更新全局变量`g_pos_x, g_pos_y`
 */
void position_calc(void) {
    static int64_t last_encoder_position[2]; /* 上次编码器的值, 做差 */

    static float32_t enc_axis[2];
    static float32_t enc_delta[2][2] = {{0.0f, 0.0f}, {0.0f, 0.0f}};
    static float32_t enc_transfer[2] = {enc_cali_cos[0] * PI * enc_diameter[0],
                                        enc_cali_cos[1] * PI * enc_diameter[1]};

    /* 编码器差值转换到坐标点矩阵 */
    const static arm_matrix_instance_f32 enc_transfer_matrix = {
        .numCols = 1, .numRows = 2, .pData = enc_transfer};
    /* 编码器差值矩阵 */
    const static arm_matrix_instance_f32 enc_delta_matrix = {
        .numCols = 2, .numRows = 2, .pData = &enc_delta[0][0]};
    /* 编码器坐标系矩阵 */
    static arm_matrix_instance_f32 enc_axis_matrix = {
        .numCols = 1, .numRows = 2, .pData = &enc_axis[0]};

    static float32_t real_axis[2];
    static float32_t real_transfer[2][2];

    /* 全场定位坐标系矩阵 */
    static arm_matrix_instance_f32 real_axis_matrix = {
        .numCols = 1, .numRows = 2, .pData = &real_axis[0]};
    const static arm_matrix_instance_f32 real_transfer_matrix = {
        .numCols = 2, .numRows = 2, .pData = &real_transfer[0][0]};

    /* 更新编码器差值矩阵 */
    as5047_get_relative_position(0);
    as5047_get_relative_position(1);
    /* 编码器 x 轴需要反一下 */
    enc_delta[0][0] = (float)(g_as5047_position[0].total_position -
                              last_encoder_position[0]) /
                      (float)AS5047_CIRCLE_CNT;
    enc_delta[1][1] = (float)(last_encoder_position[1] -
                              g_as5047_position[1].total_position) /
                      (float)AS5047_CIRCLE_CNT;
    last_encoder_position[0] = g_as5047_position[0].total_position;
    last_encoder_position[1] = g_as5047_position[1].total_position;

    /* 更新编码器坐标系 */
    arm_mat_mult_f32(&enc_delta_matrix, &enc_transfer_matrix, &enc_axis_matrix);

    /* 更新实际坐标系转换矩阵 */
    /* 将 yaw 角度限定在 -180 ~ 180 度 */
    if (g_pos_yaw > 180.0f) {
        g_pos_yaw -= 360.0f;
    } else if (g_pos_yaw < -180.0f) {
        g_pos_yaw += 360.0f;
    }

    g_pos_yaw = g_wit_angle[WIT_YAW] + g_pos_yaw_offset;
    float32_t sin_val, cos_val;
    arm_sin_cos_f32(-(g_pos_yaw + 45.0f), &sin_val, &cos_val);

    real_transfer[0][0] = cos_val;
    real_transfer[0][1] = sin_val;
    real_transfer[1][0] = -sin_val;
    real_transfer[1][1] = cos_val;

    /* 转换到实际坐标系 */
    arm_mat_mult_f32(&real_transfer_matrix, &enc_axis_matrix,
                     &real_axis_matrix);
    /* 坐标积分 */
    g_pos_x += real_axis[0];
    g_pos_y += real_axis[1];
}
