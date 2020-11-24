#!/usr/bin/env python
# -*- coding: utf-8 -*-
# -*- Python -*-

# # ガウス過程による複数歩行者の軌跡予測

# データの前処理に以下の処理を追加
# * 歩行者一人について観測された3点のうち、前回のデータとの差がしきい値以内のものを採用する
#     * 3点すべてにおいてしきい値を上回る場合、その歩行者についてはデータの追加を行わない

# - 入力ファイルの作成<br>
# \$ cat data3-1_org.txt | awk '{print NR,",",$0}' > data3-1.txt

"""
 @file gaussian_process_multi.py
 @brief ModuleDescription
 @date $Date$


"""
# 必要なライブラリのインポート
import numpy as np
import csv
import matplotlib.pyplot as plt
from matplotlib import animation, rc
from IPython.display import HTML
import pandas as pd
import datetime

import OpenRTM_aist
import RTC
import sys
import time
sys.path.append(".")

# Import RTM module


# Import Service implementation class
# <rtc-template block="service_impl">

# </rtc-template>

# Import Service stub modules
# <rtc-template block="consumer_import">
# </rtc-template>


# This module's spesification
# <rtc-template block="module_spec">
gaussian_process_multi_spec = ["implementation_id", "gaussian_process_multi",
                               "type_name",         "gaussian_process_multi",
                               "description",       "ModuleDescription",
                               "version",           "1.0.0",
                               "vendor",            "TajimaNina",
                               "category",          "Category",
                               "activity_type",     "STATIC",
                               "max_instance",      "1",
                               "language",          "Python",
                               "lang_type",         "SCRIPT",
                               "conf.default.parameter", "0",

                               "conf.__widget__.parameter", "text",

                               "conf.__type__.parameter", "long",

                               ""]
# </rtc-template>

##
# @class gaussian_process_multi
# @brief ModuleDescription
#
##

# ガウシアン・カーネル


class GaussianKernel(object):
    def __init__(self, params):
        assert np.shape(params) == (2,)
        self.__params = params

    def get_params(self):
        return np.copy(self.__params)

    # カーネル関数の値を計算
    def __call__(self, x, y):
        return self.__params[0] * np.exp(-0.5 * self.__params[1] * (x - y)**2)

    # カーネル関数のパラメータでの微分を計算（今回は未使用）
    def derivatives(self, x, y):
        sq_diff = (x - y)**2
        delta_0 = np.exp(-0.5 * self.__params[1] * sq_diff)
        delta_1 = -0.5 * sq_diff * delta_0 * self.__params[0]
        return (delta_0, delta_1)

    # カーネル関数のパラメータを更新（今回は未使用）
    def update_parameters(self, updates):
        assert np.shape(updates) == (2,)
        self.__params += updates

# ガウス過程による回帰


class GaussianProcessRegression(object):
    def __init__(self, kernel, beta=1.):
        self.kernel = kernel
        self.beta = beta

    # 与えられたパラメータでの回帰
    def fit(self, x, t):
        self.x = x
        self.t = t
        # グラム行列の計算
        Gram = self.kernel(*np.meshgrid(x, x))
        # 共分散行列の計算
        self.covariance = Gram + np.identity(len(x)) / self.beta
        # 精度行列の計算
        self.precision = np.linalg.inv(self.covariance)

    # 学習フェーズ（今回は未使用，学習しない代わりに処理が速い）
    # learning_rate = 0.1 iter_max = 10000
    def fit_kernel(self, x, t, learning_rate=0.1, iter_max=10000):
        for i in range(iter_max):
            params = self.kernel.get_params()
            # カーネル関数の今のパラメータで回帰
            self.fit(x, t)
            # 対数エビデンス関数をパラメータで微分
            gradients = self.kernel.derivatives(*np.meshgrid(x, x))
            # パラメータの更新量を計算
            updates = np.array([-np.trace(self.precision.dot(grad))
                                + t.dot(self.precision.dot(grad).dot(self.precision).dot(t)) for grad in gradients])
            # パラメータを更新
            self.kernel.update_parameters(learning_rate * updates)
            # 終了判定
            if np.allclose(params, self.kernel.get_params()):
                break
        else:
            print("parameters may not have coveraged")

    # 推論フェーズ
    def predict_dist(self, x):
        K = self.kernel(*np.meshgrid(x, self.x, indexing='ij'))
        # 予測分布の平均を計算
        mean = K.dot(self.precision).dot(self.t)
        # 予測分布の分散を計算
        var = self.kernel(x, x) + 1 / self.beta - \
            np.sum(K.dot(self.precision) * K, axis=1)
        return mean.ravel()

# 追加可能なデータか判定(データの前処理で使用)


def can_add(prev_x, prev_y, new_x, new_y, interval):
    th = 1000  # 1ステップごとのしきい値

    prev_x = prev_x*3000
    prev_y = prev_y*2000+2000

    # 前回のデータと新しく観測したデータ間の距離を計算
    distance = np.sqrt((new_x - prev_x)**2 + (new_y - prev_y)**2)

    # 距離がしきい値以内の場合に追加可能なデータと判定 (intervalは前回の観測からの間隔)
    if distance <= th*interval:
        return True
    else:
        return False

# データの前処理


def pre_data(filename, num):

    t, x, y = [[] for i in range(num)], [[]
                                         for i in range(num)], [[] for i in range(num)]

    with open(filename) as f:
        reader = csv.reader(f)

        # 外れ値への対処 + 測定領域の値での正規化
        for row in reader:
            for i in range(num):
                if float(row[(2 * i) + 1]) != 100000.0:
                    new_x = float(row[(2 * i) + 1])
                    new_y = float(row[(2 * i) + 2])
                    if len(t[i]) == 0 or can_add(x[i][-1], y[i][-1], new_x, new_y, int(row[0]) - t[i][-1]):
                        t[i] = np.append(t[i], int(row[0]))
                        x[i] = np.append(x[i], new_x/3000.0)
                        y[i] = np.append(y[i], (new_y - 2000)/2000.0)
                        continue

                if (float(row[(num * 2) + (4 * i) + 1]) != 0 and float(row[(num * 2) + (4 * i) + 1]) != 100000.0) and (float(row[(num * 2) + (4 * i) + 3]) != 0 and float(row[(num * 2) + (4 * i) + 3]) != 100000.0):
                    new_x = (float(row[(num * 2) + (4 * i) + 1]) +
                             float(row[(num * 2) + (4 * i) + 3]))/2.0
                    new_y = (float(row[(num * 2) + (4 * i) + 2]) +
                             float(row[(num * 2) + (4 * i) + 4]))/2.0
                    if len(t[i]) == 0 or can_add(x[i][-1], y[i][-1], new_x, new_y, int(row[0]) - t[i][-1]):
                        t[i] = np.append(t[i], int(row[0]))
                        x[i] = np.append(x[i], new_x/3000.0)
                        y[i] = np.append(y[i], (new_y - 2000)/2000.0)
                        continue

                if (float(row[(num * 2) + (4 * i) + 1]) != 0 and float(row[(num * 2) + (4 * i) + 1]) != 100000.0) and (float(row[(num * 2) + (4 * i) + 3]) == 0 or float(row[(num * 2) + (4 * i) + 3]) == 100000.0):
                    new_x = float(row[(num * 2) + (4 * i) + 1])
                    new_y = float(row[(num * 2) + (4 * i) + 2])
                    if len(t[i]) == 0 or can_add(x[i][-1], y[i][-1], new_x, new_y, int(row[0]) - t[i][-1]):
                        t[i] = np.append(t[i], int(row[0]))
                        x[i] = np.append(x[i], new_x/3000.0)
                        y[i] = np.append(y[i], (new_y - 2000)/2000.0)
                        continue

                if (float(row[(num * 2) + (4 * i) + 1]) == 0 or float(row[(num * 2) + (4 * i) + 1]) == 100000.0) and (float(row[(num * 2) + (4 * i) + 3]) != 0 and float(row[(num * 2) + (4 * i) + 3]) != 100000.0):
                    new_x = float(row[(num * 2) + (4 * i) + 3])
                    new_y = float(row[(num * 2) + (4 * i) + 4])
                    if len(t[i]) == 0 or can_add(x[i][-1], y[i][-1], new_x, new_y, int(row[0]) - t[i][-1]):
                        t[i] = np.append(t[i], int(row[0]))
                        x[i] = np.append(x[i], new_x/3000.0)
                        y[i] = np.append(y[i], (new_y - 2000)/2000.0)

    return t, x, y

# データの後処理


def post_data(x, y, x_pred, y_pred):
    x = x * 3000
    y = y * 2000 + 2000
    x_pred = x_pred * 3000
    y_pred = y_pred * 2000 + 2000
    return x, y, x_pred, y_pred


def main_processing(filename, num):

    # 変数の初期化
    kernel, regression = [], []
    t_train, x_train, y_train = [[] for i in range(num)], [[] for i in range(num)], [
        [] for i in range(num)]
    x_pred, y_pred = [[] for i in range(num)], [[] for i in range(num)]

    # データの前処理
    t_train, x_train, y_train = pre_data(filename, num)

    # ガウス過程によるデータの回帰
    for i in range(num * 2):
        kernel.append(GaussianKernel(params=np.array([0.01, 0.01])))
        regression.append(GaussianProcessRegression(
            kernel=kernel[i], beta=100))
        if i % 2 == 0:
            regression[i].fit(t_train[int(i/2)], x_train[int(i/2)])
        else:
            regression[i].fit(t_train[int((i - 1)/2)], y_train[int((i - 1)/2)])

    # 推論フェーズ
    t_test = np.linspace(0, 200, 200)
    # t_test = np.linspace(0, 175, 175)
    for i in range(num):
        x_pred[i] = regression[2 * i].predict_dist(t_test)
        y_pred[i] = regression[(2 * i) + 1].predict_dist(t_test)

    # データの後処理
    for i in range(num):
        x_train[i], y_train[i], x_pred[i], y_pred[i] = post_data(
            x_train[i], y_train[i], x_pred[i], y_pred[i])

    return t_test, x_pred, y_pred


"""
# アニメーションの一ステップごとの処理


def one_step(i, x, y, num):
    for j in range(num):
        dots[j].set_data(x[j][i], y[j][i])
    return dots

# アニメーションの見た目の設定


def draw(num):
    dots = []
    fig, ax = plt.subplots()
    plt.close()
    for i in range(num):
        dot, = ax.plot([], [], marker='o')
        dots.append(dot)
    ax.set_xlim(- 4000, 4000)
    ax.set_ylim(- 0, 4000)
    ax.set_aspect('equal')
    ax.axes.xaxis.set_visible(False)
    ax.axes.yaxis.set_visible(False)
    return fig, dots
"""


class gaussian_process_multi(OpenRTM_aist.DataFlowComponentBase):
    ##
    # @brief constructor
    # @param manager Maneger Object
    #
    def __init__(self, manager):
        OpenRTM_aist.DataFlowComponentBase.__init__(self, manager)
        self._d_flag = RTC.TimedString(RTC.Time(0, 0), "")
        """
				"""
        self._flagIn = OpenRTM_aist.InPort("flag", self._d_flag)
        self._d_filename = RTC.TimedString(RTC.Time(0, 0), "")
        """
				"""
        self._filenameIn = OpenRTM_aist.InPort("filename", self._d_filename)
        self._d_human_in = RTC.TimedLong(RTC.Time(0, 0), 0)
        """
		"""
        self._human_inIn = OpenRTM_aist.InPort(
            "human_in", self._d_human_in)
        #self._d_human_out = RTC.TimedLong(RTC.Time(0, 0), 0)
        """
		"""
        #self._d_human_outIn = OpenRTM_aist.InPort("human_out", self._d_human_out)
        self._d_outport = RTC.TimedLongSeq(RTC.Time(0, 0), [])
        """
		"""
        self._outportOut = OpenRTM_aist.OutPort("outport", self._d_outport)

        # initialize of configuration-data.
        # <rtc-template block="init_conf_param">
        """

		- Name:  parameter
		- DefaultValue: 0
		"""
        self._parameter = [0]

        # </rtc-template>

    def onInitialize(self):
        # Bind variables and configuration variable
        self.bindParameter("parameter", self._parameter, "0")

        # Set InPort buffers
        self.addInPort("flag", self._flagIn)
        self.addInPort("filename", self._filenameIn)
        self.addInPort("human_in", self._human_inIn)

        #self.addInPort("human_out", self._d_human_outIn)

        # Set OutPort buffers
        self.addOutPort("outport", self._outportOut)

        # Set service provider to Ports

        # Set service consumers to Ports

        # Set CORBA Service Ports
        self._log_list = []
        return RTC.RTC_OK

    def onActivated(self, ec_id):
        self.count = 0
        return RTC.RTC_OK

    def onDeactivated(self, ec_id):
        self.count = 0
        return RTC.RTC_OK

    def onExecute(self, ec_id):
        if (self._flagIn.isNew()):
            self._d_flag = self._flagIn.read()
            self._flag = self._d_flag.data
            print("flag:", self._flag)
            if (self._flag == "g_start"):
                if (self._human_inIn.isNew()):
                    # time.sleep(10)
                    # 人数ポート読み込み
                    self._d_human_in = self._human_inIn.read()
                    self._human_in = self._d_human_in.data
                    print("human_in:", self._human_in)
                    if (self._human_in != 0):
                        self._d_filename = self._filenameIn.read()
                        self._filename = self._d_filename.data
                        print("filename:", self._filename)
                        #self._d_human_out = self._d_human_outIn.read()
                        #self._human_out = self._d_human_out.data
                        #print("human_out:", self._human_out)
                        self._people_num = int(self._human_in)
                        input_file = self._filename
                        person_num = self._people_num
                        t, x, y = main_processing(input_file, person_num)

                        # 結果の出力
                        for i in range(len(t)):
                            # ステップごとに、人数分のx座標とy座標のリストを出力
                            print([x[j][i] for j in range(person_num)], [y[j][i]
                                                                         for j in range(person_num)])
                            for j in range(person_num):
                                self._log_list.append(x[j][i])
                                self._log_list.append(y[j][i])
                            self._logfile = self._time.strftime(
                                'C:\workspace\gaussian_process_multi\data\log_human' + str(self._people_num) + '_%Y%m%d_%H%M%S.csv')

                            with open(self._logfile, 'a') as f:
                                writer = csv.writer(f, lineterminator='\n')
                                writer.writerow(self._log_list)
                                self._log_list = []
                                # [x[0][i]]+[y[0][i]]+[x[1][i]]+[y[1][i]]+[x[2][i]]+[y[2][i]]+[x[3][i]]+[y[3][i]]+[x[4][i]]+[y[4][i]]+[x[5][i]]+[y[5][i]]+[x[6][i]]+[y[6][i]]+[x[7][i]]+[y[7][i]]+[x[8][i]]+[y[8][i]]+[x[9][i]]+[y[9][i]])

                        # for j in range(person_num):
                        #    for i in range(len(t)):
                        #        self._log_list[2*j][i] = x[j][i]
                        #        self._log_list[2*j+1][i] = y[j][i]
                        # self._log_list[0 + 2*(self.count-1)] = x
                        # self._log_list[1 + 2*(self.count-1)] = y

                        # with open('C:\workspace\gaussian_process_multi\data\log_human'+str(self._people_num) + '.csv', 'a') as f:
                        #    writer = csv.writer(f, lineterminator='\n')
                        #    writer.writerow(self._log_list)
                        #self.count += 1
            elif (self._flag == "g_stop"):
                print("g_stop")
                self._time = datetime.datetime.now()
            else:
                print("?????error??????")

        return RTC.RTC_OK


def gaussian_process_multiInit(manager):
    profile = OpenRTM_aist.Properties(defaults_str=gaussian_process_multi_spec)
    manager.registerFactory(
        profile, gaussian_process_multi, OpenRTM_aist.Delete)


def MyModuleInit(manager):
    gaussian_process_multiInit(manager)

    # Create a component
    comp = manager.createComponent("gaussian_process_multi")


def main():
    mgr = OpenRTM_aist.Manager.init(sys.argv)
    mgr.setModuleInitProc(MyModuleInit)
    mgr.activateManager()
    mgr.runManager()


if __name__ == "__main__":
    main()
