﻿// -*- C++ -*-
/*!
 * @file  Human_Tracking.cpp
 * @brief ModuleDescription
 * @date $Date$
 *
 * @author Daiki Nomiyama, Shinichi Ishida, Shogo Namatame, Nobuto Matsuhira / SHIBAURA INSTITUTE OF TECHNOLOGY /  3-7-5 TOYOSU KOUTO-KU TOKYO 135-8548
 *　　　　 Email:md14067@shibaura-it.ac.jp
 *         Tell : 03-5859-8073
 * @author 野見山大基　芝浦工業大学　135-8548 東京都江東区豊洲3-7-5
 * 
 *
 *
 * ライセンス：aaaaag
 * 以下のMIT Licenceを適用します．
 * ////////////////////////////////////////////////////////////////////////////////////////////////////

	Copyright (c) 2003-2009, MIST Project, Intelligent Media Integration COE, Nagoya University
	All rights reserved.

	Redistribution and use in source and binary forms, with or without modification,
	are permitted provided that the following conditions are met:

		1. Redistributions of source code must retain the above copyright notice,
		   this list of conditions and the following disclaimer. 

		2. Redistributions in binary form must reproduce the above copyright notice,
		   this list of conditions and the following disclaimer in the documentation
		   and/or other materials provided with the distribution.

		3. Neither the name of the Nagoya University nor the names of its contributors
		   may be used to endorse or promote products derived from this software
		   without specific prior written permission. 

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
	IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
	FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
	CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
	DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
	IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
	THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

	//////////////////////////////////////////////////////////////////////////////////////////////////////
 * http://mist.suenaga.cse.nagoya-u.ac.jp/trac/wiki/License (Last accessed 2014/09/24)
 *
 *
 * $Id$
 */

#include "stdafx.h"
#include "Human_Tracking.h"
#include "palette_window.h"
#include <math.h>
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <time.h>
#include <mmsystem.h>
#pragma once
#pragma comment(lib,"winmm.lib")

#define M_PI            3.14159265358979323846
#define max_particles   100
#define cut_rate        0.6
#define NUM_Hmax        11
FILE *fp;
//#define m_Human_set 300 // 2800
double d_stride=800;
double sigma= 60;
int first_flg=1;
int first_flgA=1;
DWORD  stime;
int pre_human_num = 0;

using namespace std;


// Module specification
// <rtc-template block="module_spec">
static const char* human_tracking_spec[] =
  {
    "implementation_id", "Human_Tracking",
    "type_name",         "Human_Tracking",
    "description",       "ModuleDescription",
    "version",           "1.0.0",
    "vendor",            "Nomiyama",
    "category",          "Category",
    "activity_type",     "PERIODIC",
    "kind",              "DataFlowComponent",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "compile",
    // Configuration variables
    "conf.default.Human_NUM", "10",
    "conf.default.Environmental_maxdata", "4000",
    "conf.default.LRFdata_min", "44",
    "conf.default.LRFdata_max", "725",
	"conf.default.LRFdata_mid", "384",
	"conf.default.LRF_Total_steps", "1024",
	"conf.default.LRFdata_theta", "360",
    "conf.default.Human_width", "200",
    "conf.default.Count_mode", "IN,OUT",
	"conf.default.DataRecord", "ON",
	"conf.default.Human_setdistance", "2800",
	"conf.default.Human_TrackingData", "2500",
	"conf.default.Count_area_distance", "2500",
    // Widget
    "conf.__widget__.Human_NUM", "text",
    "conf.__widget__.Environmental_maxdata", "text",
    "conf.__widget__.LRFdata_min", "text",
    "conf.__widget__.LRFdata_max", "text",
	"conf.__widget__.LRFdata_mid", "text",
	"conf.__widget__.LRFdata_theta", "text",
	"conf.__widget__.LRF_Total_steps", "text",
    "conf.__widget__.Human_width", "text",
    "conf.__widget__.Count_mode", "text",
	"conf.__widget__.Human_setdistance", "text",
	"conf.__widget__.Human_TrackingData", "text",
	"conf.__widget__.Count_area_distance", "text",
	"conf.__widget__.DataRecord", "text",
    // Constraints
    ""
  };
// </rtc-template>

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
Human_Tracking::Human_Tracking(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_range_dataIn("range_data", m_range_data),
	m_humanXOut("human_dataX", m_humanX),
	m_humanYOut("human_dataY", m_humanY),
	//Tajima20200914
	m_humanEdge1xOut("human_edge1x", m_humanEdge1x),
	m_humanEdge1yOut("human_edge1y", m_humanEdge1y),
	m_humanEdge2xOut("human_edge2x", m_humanEdge2x),
	m_humanEdge2yOut("human_edge2y", m_humanEdge2y),

	m_humanVOut("human_dataV", m_humanV),
	m_humanTOut("human_dataT", m_humanT),
	m_hcountOut("count_data", m_hcount),
	m_human_in_numberOut("human_In",m_human_in_number), //20190710追記Kusaka
	m_human_out_numberOut("human_Out",m_human_out_number), //20190710追記Kusaka
	m_human_wait_numberOut("Congestion degree", m_human_wait_number)
	//m_pointOut("Point_data",m_point)
    // </rtc-template>
{
}

/*!
 * @brief destructor
 */
Human_Tracking::~Human_Tracking()
{
}



RTC::ReturnCode_t Human_Tracking::onInitialize()
{ 
  addInPort("range_data", m_range_dataIn);
  addOutPort("human_dataX", m_humanXOut);
  addOutPort("human_dataY", m_humanYOut);
  //tajima20200914
  addOutPort("human_edge1x", m_humanEdge1xOut);
  addOutPort("human_edge1y", m_humanEdge1yOut);
  addOutPort("human_edge2x", m_humanEdge2xOut);
  addOutPort("human_edge2y", m_humanEdge2yOut);
 // addOutPort("human_dataV", m_humanVOut);
 // addOutPort("human_dataT", m_humanTOut);
  addOutPort("count_data", m_hcountOut);
  addOutPort("human_In",m_human_in_numberOut); //20190710追記Kusaka
  addOutPort("human_Out",m_human_out_numberOut); //20190710追記Kusaka
  addOutPort("Congestion degree", m_human_wait_numberOut); //混雑度(1=小、2=中、3=大の三段階で表示)
  //addOutPort("Point_data", m_pointOut);
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  
  // Set OutPort buffer
  
  // Set service provider to Ports
  
  // Set service consumers to Ports
  
  // Set CORBA Service Ports
  
  // </rtc-template>

  // <rtc-template block="bind_config">
  // Bind variables and configuration variable
  bindParameter("Human_NUM", m_NUM_max, "10");
  bindParameter("Environmental_maxdata", m_environment, "4000");
  bindParameter("LRFdata_min", m_LRFdata_min, "44");
  bindParameter("LRFdata_max", m_LRFdata_max, "725");
  bindParameter("LRFdata_mid", m_LRFdata_mid, "384");
  bindParameter("LRFdata_theta", m_LRFdata_theta, "360");
  bindParameter("LRF_Total_steps", m_LRF_steps, "1081");
  bindParameter("Human_width", m_human_width, "200");
  bindParameter("Count_mode", m_Count_mode, "IN,OUT");
  bindParameter("DataRecord", m_csvdata, "ON");
  bindParameter("Count_area_distance", m_Count_area, "2500");
  bindParameter("Human_setdistance", m_Human_set, "2800");
  bindParameter("Human_TrackingData", m_Human_TrackingData, "2500");
  
  // </rtc-template>
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t Human_Tracking::onFinalize()
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Human_Tracking::onStartup(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Human_Tracking::onShutdown(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

RTC::ReturnCode_t Human_Tracking::onActivated(RTC::UniqueId ec_id)
{ 
	 CSV_CHEAK();       //CSV読み込み用
	 window_init();       //描写ウィンドウ生成
     Human_Initialize();  //初期化
	 open_check = false;

	 log_open();//20190804kusaka

	//m_human_wait_num=0;
	// m_human_wait_num=2;
	 first_flg=1;
	 std::cout<<"on Activate"<<std::endl;
  return RTC::RTC_OK;
}


RTC::ReturnCode_t Human_Tracking::onDeactivated(RTC::UniqueId ec_id)
{
	delete [] m_qHuman;  //各人の歩行情報（構造体）データ解放
	delete [] m_qHuman2; //各人の歩行情報（構造体）データ解放
	delete [] m_data;    //環境データ及び生データ解放
	
	log_close();//20190804kusaka
	std::cout<<"Deactivate state"<<std::endl;
  return RTC::RTC_OK;
}


RTC::ReturnCode_t Human_Tracking::onExecute(RTC::UniqueId ec_id)
{
	if(m_range_dataIn.isNew())
	{	
		TIME_SET();
		LRFData_In();						//新しいデータを取得
		strong_pass_filter(10,6);			//密度が低過ぎるデータの有効フラグを折る（ごみ取り）{前後のスキャン範囲,有効データ数}以下のデータが対象
		weak_add_filter(5,5);				//密度が高い場所にあるデータの欠落を補完（穴埋め）
		parson_devid_method(10,4,3,300);	//サブ計測結果として得られたデータをオブジジェェクトごとに分ける
		LRFPocoHuman();						//マッチングによる複数人追跡

		//表示部分
		copy_data2(m_data,m_NUM_max);		//LRFデータ（ステップ数など）を描写関数にコピー
		copy_data(m_range_data);			//LRFデータを描写関数にコピー
		copy_data3(m_qHuman);				//人データを描写関数にコピー

		Data_outport();
		//Data_outport();						//ＯＵＴＰＯＲＴにデータを渡す
		
		if(m_csvdata=="ON"){
			//Logsave_Input(1,_T("COUNTDATA.csv"),m_human_in,m_human_out,0,0,0,0,0,0);//ＩＮ，ＯＵＴ，ＷＡＩＴデータを保存//20190804Kusaka
			//Logsave_Input2(2,_T("生データ.csv"));	//LRFの生データを保存　１０秒で3.5ＭＢ
		}//20190804kusaka
	}
	else{}
	//Logsave_Input2(2,_T("生データ.csv"));	//LRFの生データを保存　１０秒で3.5ＭＢ
	//Logsave_Input(1,_T("20150722実験01.csv"),m_human_in,m_human_out,0,0,0,0,0,0);//ＩＮ，ＯＵＴ，ＷＡＩＴデータを保存

	time_t timer;
		struct tm *t_st;
		time(&timer);
		t_st = localtime(&timer);

		char filename[50];
	//fprintf(fp,"%d:%d:%d,%d,%d",t_st->tm_hour,t_st->tm_min,t_st->tm_sec, m_human_in , m_human_out );
	//fprintf(fp,"\r");
	window_reload();					//描写更新	
	copy_data4(timenow);//現時刻の文字データを描写関数にコピー

	/*if(first_flg==1){
		first_flg=0;
		Data_outport();	//ＯＵＴＰＯＲＴにデータを渡す
		//m_human_wait_numberOut:
		printf("output\n");
		stime = timeGetTime();  //基準時刻
	}
	else{
        DWORD etime = timeGetTime();           // 現在時間

		DWORD dtime = etime-stime;
		if(dtime>=5*60*1000){  //変更できるように

			Data_outport();
			//m_human_wait_numberOut;
			printf("output\n");
			stime=etime;
		}
	}*/
	Data_outport();						//ＯＵＴＰＯＲＴにデータを渡す

   return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t Human_Tracking::onAborting(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Human_Tracking::onError(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Human_Tracking::onReset(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Human_Tracking::onStateUpdate(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Human_Tracking::onRateChanged(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

//新データ整理関数/生データ，及び環境データ仕分け関数
void Human_Tracking::LRFData_In(){

	m_data->LRF_start=m_LRFdata_min;                                           //描写用に値を保存
	m_data->LRF_end=m_LRFdata_max;
	m_data->LRF_mid=m_LRFdata_mid;
	m_data->LRF_theta=m_LRFdata_theta;
	m_data->LRF_steps=m_LRF_steps;

	m_range_dataIn.read();
		for(int count = m_LRFdata_min;count <= m_LRFdata_max;count++)	        //得られたデータを保存変数に順次代入してゆく
		{
		m_data->mem_data_x[count]=m_range_data.ranges[count];
		m_data->mem_data_x4[count]=m_range_data.ranges[count];
		//std::cout<<count<<" : "<<m_range_data.ranges[count]<<std::endl;
		if(m_data->mem_data_x[count]>m_environment)m_data->mem_data_x[count]=m_environment;
		if(m_data->mem_data_x[count]<0)m_data->mem_data_x[count]=0;
		if(m_data->mem_data_x2[count]==0)m_data->mem_data_x2[count]=m_data->mem_data_x[count];
		if(m_data->mem_data_x2[count]!=0){                                      //有効データが得られた場合（データが0でないとき）
			if(m_data->mem_data_x[count]>m_data->mem_data_x2[count]){
				m_data->mem_data_x2[count]=m_data->mem_data_x[count];           //もとのデータよりも距離の大きいデータがえられた場合は情報を更新する
			}
			else{}

			if(m_data->mem_data_x[count]/m_data->mem_data_x2[count]<0.94){		//元データよりも9割以下の場合は人がいると判断（背景差分法）
				if(m_data->mem_data_x[count]>10){		                        //データが10cm以下の場合は無視（ノイズ除去のため）
					m_data->m_different_search[count]=1;		                //データが有効な場所のフラグを立てる(1ステップごとに行う．1：有効，0:無効)
				}
				else{
				m_data->m_different_search[count]=0;			                 //無効データ場合はフラグを折る
					}
				}
			else{
			m_data->m_different_search[count]=0;			                     //無効データ場合はフラグを折る
			}
						
		}
		//基本となるデータが0のとき
		else{
			if(m_data->mem_data_x[count]=m_environment){
				if(m_data->mem_data_x2[count]=m_environment){
					m_data->m_different_search[count]=0;	
				}
			}

		}
		if(m_data->mem_data_x2[count]!=0 && m_data->mem_data_x[count]==0)m_data->mem_data_x[count]=m_data->mem_data_x2[count];
	}
}
//密度の低い部分を除外する関数
void Human_Tracking::strong_pass_filter(int scan_range,int filter_limit)
{
	int filter_on=0;
	//密度の弱いデータを常にフィルタ処理
	for(int i1 = m_LRFdata_min;i1 <= m_LRFdata_max;i1++){
		filter_on=0;
		if(m_data->m_different_search[i1]==1){
			for(int filter_count=1;filter_count<scan_range;filter_count++){		//各データの前後10カウントを検査
				if(m_data->m_different_search[i1-filter_count]==1)filter_on++;
				if(m_data->m_different_search[i1+filter_count]==1)filter_on++;
								
			}
			if(filter_on<filter_limit){			//検査領域に有効データが10個以下な場合はデータを無効化（ノイズの除去）
			m_data->m_different_search[i1]=0;
			move_trace[0][0]-=1;
			move_trace[0][1]-=i1;
			}
		}
	}

}
//密度の高い箇所にある欠落データを補完する関数
void Human_Tracking::weak_add_filter(int scan_range,int filter_limit)
{
	
		double mem_data_x_ave=0;
	//距離平均値から大きく外れる値のフラグを折る
			for(int i1 = m_LRFdata_min;i1 <= m_LRFdata_max;i1++){
		if(m_data->m_different_search[i1]==1){
		for(int filter_count2=1;filter_count2<=scan_range;filter_count2++){		//各データの前10カウントを検査
			mem_data_x_ave+=m_data->mem_data_x[i1];
		}	
			mem_data_x_ave=mem_data_x_ave/scan_range;	
		for(int filter_count=1;filter_count<=scan_range;filter_count++){		
			if(abs(mem_data_x_ave-m_data->mem_data_x[i1+filter_count])>300)m_data->m_different_search[i1+filter_count]==0;
		}
		}
			
		else{}
			}

}
//複数のオブジェクトを分割する関数
void Human_Tracking::parson_devid_method(int scan_range,int filter_limit,int delsize,int LimitAveDistance)
{
	int parson_X=1;
	//背景差分法により得られた1ステップごとの有効データの連続性を確認
	for(int i1 = m_LRFdata_min;i1 <= m_LRFdata_max;i1++){
		//std::cout << "Data is " <<m_data->m_different_search[340]<< std::endl;
		if(m_data->m_different_search[i1]==1){
			trans_memdata_to_pxpy(i1);
			human_sx[i1] = ppx_d;
			human_sy[i1] = ppy_d;
			trans_memdata_to_pxpy(i1+1);
			human_ex[i1+1] = ppx_d;
			human_ey[i1+1] = ppy_d;
			human_dr[i1]=hypot((human_sx[i1]-human_ex[i1+1]),(human_sy[i1]-human_ey[i1+1]));
			
			if(m_data->m_different_search[i1]-m_data->m_different_search[i1-1]==1){
				flag_check_start[parson_X]=i1;
			}
			else if(m_data->m_different_search[i1]-m_data->m_different_search[i1+1]==0){
				if(human_dr[i1]>200){
					flag_check_end[parson_X]=i1;
				}
				else if(human_dr[i1-1]>200&&(m_data->m_different_search[i1]-m_data->m_different_search[i1-1]==0)){
					flag_check_start[parson_X]=i1;
				}
			}
			else if(m_data->m_different_search[i1]-m_data->m_different_search[i1+1]==1){
				if(human_dr[i1-1]>200){
					flag_check_start[parson_X]=0;
				}
				else{
					flag_check_end[parson_X]=i1;
				}
			}
			else {}
			
		}
		/*if((m_data->m_different_search[i1]-m_data->m_different_search[i1-1])==1){
			flag_check_start[parson_X]=i1;
		}
		else if((m_data->m_different_search[i1]-m_data->m_different_search[i1-1])==-1){
			flag_check_end[parson_X]=i1-1;
		}
		else{}*/
	//	std::cout << "Data is " <<flag_check_start[1]<<","<<flag_check_end[1]<< std::endl;
			
		//ある程度連続した有効データに対して連続した有効ステップの幅を計算
		if(flag_check_start[parson_X]!=0 && flag_check_end[parson_X]!=0 && flag_check_end[parson_X] != flag_check_start[parson_X] && flag_check_end[parson_X] > flag_check_start[parson_X] && flag_check_start[parson_X]>0 &&flag_check_end[parson_X]>0){
			
			trans_memdata_to_pxpy(flag_check_start[parson_X]);
			human_sx[parson_X] = ppx_d;
			human_sy[parson_X] = ppy_d;

			trans_memdata_to_pxpy(flag_check_end[parson_X]);
			human_ex[parson_X] = ppx_d;
			human_ey[parson_X] = ppy_d;

			human_dr[parson_X] = hypot((human_sx[parson_X]-human_ex[parson_X]),(human_sy[parson_X]-human_ey[parson_X]));//人幅
		//ある程度の幅（コンフィギュレーションパラメーター変更可能）移動で物体と認識
			if(parson_X<=m_NUM_max){
				int i=0;
				//for(int i=0 ; i<=m_NUM_max ; i++){
					//for(int i2=0;i2<max_particles;i2++){
				//for (int i =0; i<m_NUM_max; i++){
				if( human_dr[parson_X]>m_human_width/* && m_qHuman->human[i].distance2[1]>0 && m_qHuman->human[i].distance2[2]>0 *//*&& m_qHuman->human[i].velocity[i2]>=0*/){
					for(int i =flag_check_start[parson_X];i<=flag_check_end[parson_X];i++){		
						m_data->parson_divide[parson_X][i]=1;
					}
						move_trace[parson_X][0]=flag_check_start[parson_X];
						move_trace[parson_X][1]=flag_check_end[parson_X];
						flag_check_start[parson_X]=0;
						flag_check_end[parson_X]=0;
			        	parson_X++;	
				}	
		//ある程度の幅（コンフィギュレーションパラメーター変更可能）以下であれば有効ステップデータを無効にする．
				else{
					for(int i =flag_check_start[parson_X];i<=flag_check_end[parson_X];i++){
						m_data->parson_divide[parson_X][i]=0;
						m_data->m_different_search[i]=0;
					}
					flag_check_start[parson_X]=0;
					flag_check_end[parson_X]=0;
					move_trace[parson_X][0]=0;
					move_trace[parson_X][1]=0;		
				}
			}
			//}
			//}
			else {}
		}
		else {}
	}
	

				
}
//X，Y座標に変換する関数
void Human_Tracking::trans_memdata_to_pxpy(int count)
{
		theta=(count-m_LRFdata_mid)*m_LRFdata_theta/360*2*M_PI/m_LRF_steps;				    //ラジアン単位の角度算出
		G_dist=m_data->mem_data_x[count];			    //データをひとまず距離として保
		ppx_d=G_dist*sin(theta);
		ppy_d=G_dist*cos(theta);



}
//複数人追跡関数(重要)
void Human_Tracking::LRFPocoHuman(){

	HUMAN_STATE_CALCULATE(m_qHuman);		//計測データの更新 //対象物1からＭＡＸ10人に対して繰り返し計算
	TARGET_TRACKING(m_qHuman);				//追跡処理
	m_human_wait=0;
	for(int i=1 ; i<=m_NUM_max ; i++){

///////////パーティクルフィルタによるロックオン処理////////////////////////////////
		resample(i ,m_qHuman ,m_qHuman2);
		predict(i ,m_qHuman);
		weight(i ,m_qHuman);
		measure(i ,m_qHuman);
////////////////パーティクルフィルタによるロックオン処理ここまで///////////////////

		HUMAN_MIGRATION_LENGTH(i,m_qHuman);			//移動距離計算と入室退室カウント
		HUMAN_DELETE_COUNT(i,m_qHuman);             //入退室後や長時間残っている人データを削除するためにカウントを行う．また，追跡エリア以外で速度が小さいものも削除対象	
		}
	HUMAN_DELETE(m_qHuman);					        //入退室後や長時間残っている人データを削除する
	
}
//初期化関数
void Human_Tracking::Human_Initialize()
{
	m_qHuman = new Humans();
	m_qHuman2 = new Humans();
	m_data = new Data();

	ppx_d=0;
	ppy_d=0;
	theta=0;
	G_dist=0;
	//G_dist_edge1=0;
	//time_flag=0;


	for(int i=0 ; i<=m_NUM_max ; i++){

		for(int s2=0 ; s2<=m_NUM_max ; s2++){
		Tracking_check[i][s2]=0;
		m_qHuman->human[i].end=0;
		m_qHuman->human[i].start=0;
		m_qHuman->human[i].hum_count[s2]=0;
		distance_S2[i]=0;
		}
		for(int i2=0;i2<max_particles;i2++){
			m_qHuman->human[i].weight[i2]=0;
			m_qHuman->human[i].xp[i2]=100000;
			m_qHuman->human[i].yp[i2]=100000;
			m_qHuman->human[i].x2[i2]=100000;
			m_qHuman->human[i].y2[i2]=100000;
			m_qHuman->human[i].x3[i2]=100000;
			m_qHuman->human[i].y3[i2]=100000;
			//tajima20200916
			m_qHuman->human[i].edgex1[i2]=100000;
			m_qHuman->human[i].edgey1[i2]=100000;
			m_qHuman->human[i].edgex2[i2]=100000;
			m_qHuman->human[i].edgey2[i2]=100000;

			m_qHuman->human[i].distance2[i2]=100000;
			m_qHuman->human[i].edgedistance1[i2]=100000;
			m_qHuman->human[i].edgedistance2[i2]=100000;
			m_qHuman->human[i].vector[i2]=0;
			m_qHuman->human[i].velocity[i2]=0;
			tmp_x[i][i2]=0;
			tmp_y[i][i2]=0;
			tmp_x2[i][i2]=0;
			tmp_y2[i][i2]=0;
     	}
		m_qHuman->human[i].Hum_ID2=1;
		m_qHuman->human[i].Msg_Lv[1]=0;
		m_qHuman->human[i].Msg_Lv[2]=0;
		m_qHuman->human[i].Msg_Lv[3]=0;
		m_qHuman->human[i].theta[0]=0;
		m_qHuman->human[i].theta2[0]=0;
		//tajima20200916
		m_qHuman->human[i].edgetheta1[0]=0;
		m_qHuman->human[i].edgetheta2[0]=0;
		m_qHuman->human[i].edgedistance1[0]=0;
		m_qHuman->human[i].edgedistance2[0]=0;

		m_qHuman->human[i].distance[0]=0;
		flag_check_start[i]=0;
		flag_check_end[i]=0;
		m_human_in=0;
		m_human_in_num=0;
		m_human_out=0;
		m_human_wait=0;
		m_human_all = 0;
		px_d_other[i]=0;
		py_d_other[i]=0;
		theta_other[i]=0;
		G_dist_other[i]=0;
		G_center_angl[i]=0;
		G_dist_other[i]=0;
			
		for(int s2=0;s2<800;s2++){
			 m_data->m_different_search[s2]=0;
			 human_sx[s2]=0;
			 human_sy[s2]=0;
			 human_ex[s2]=0;
			 human_ey[s2]=0;
			 human_dr[s2]=0;
			 m_data->parson_divide[i][s2]=0;		 
		}
		for(int s3=0;s3<m_NUM_max;s3++){
			move_trace[i][s3]=0;
			distance_xy[i][s3]=0;
			distance_xy2[i][s3]=0;
			distance_xy3[i][s3]=0;
		}
	}
	for(int s3=0;s3<=10;s3++){
		leg_flag[s3]=0; 
		leg_d[s3]=0;
		m_data->xx[s3]=0;
		m_data->yy[s3]=0;
	}
	delta_x=0;
	delta_y=0;
	vx2=0;
	vx3=0;//追跡器の予測位置

}
//更新データをOUTPORTする関数
void Human_Tracking::Data_outport()
{
	int hcount_sum = 0;
	//Outポートに値を渡す
    m_humanX.data.length(m_NUM_max);
	for(int i=0;i<m_NUM_max;i++){
		m_humanX.data[i] = m_qHuman->human[i+1].y2[1];
		//m_humanX.data[i] = human_center_y[i];		//nii(2020/02/02)
		//m_humanX.data[i] = m_qHuman->human[i+1].y3[1];
		//std::cout<<"dataX[" << i << "]:"<<m_humanX.data[i]<<std::endl;
	}
	m_humanXOut.write();

	m_humanY.data.length(m_NUM_max);
	for(int i=0;i<m_NUM_max;i++){
		m_humanY.data[i] = m_qHuman->human[i+1].x2[1];
		//m_humanY.data[i] = m_qHuman->human[i+1].x3[1];
		//m_humanY.data[i] = human_center_x[i];		//nii(2020/02/02)
		//std::cout<<"dataY[" << i << "]:" <<m_humanX.data[i]<<std::endl;
	}
	m_humanYOut.write();
	
	

    m_humanV.data.length(m_NUM_max);
	for(int i=0;i<m_NUM_max;i++){
		m_humanV.data[i] = m_qHuman->human[i+1].velocity[1];
	}
	m_humanVOut.write();

	m_humanT.data.length(m_NUM_max);
	for(int i=0;i<m_NUM_max;i++){
		m_humanT.data[i] = m_qHuman->human[i+1].vector[1];
	}
	m_humanTOut.write();

	m_hcount.data.length(2);									  //////////////////////////////////
	m_hcount.data[0] = m_human_in; //右方向からの人数             ///**********変更した**********///
	m_hcount.data[1] = m_human_out; //左方向からの人数           //////////////////////////////////
	hcount_sum = m_human_in + m_human_out; //左右の合計人数
	m_human_in_number.data=m_human_in; //m_human_in_numberという変数にm_human_inに内容を書き込む//20190710追記Kusaka
	m_human_out_number.data=m_human_out;
	
	if(hcount_sum>0){
		//std::cout<<"右方向からの人数："<<m_human_in<<std::endl;//右方向からの人数表示
		//std::cout<<"左方向からの人数："<<m_human_out<<std::endl;
		//std::cout<<"合計人数："<<hcount_sum<<std::endl; //合計人数の表示
		//m_human_in_numberOut.write();
		//m_human_out_numberOut.write();
		//m_hcountOut.write(); //m_hcountOutというOutPortのオブジェクトでデータをネットワークに送り出す//20190710追記Kusaka
	}

	//m_hcount.data[2] = m_human_wait;
	//m_hcount.data[2] = count_time;
	//ol
		time_t timer;
		struct tm *t_st;
		time(&timer);
		t_st = localtime(&timer);

		char filename[50];
	
	if(first_flgA==1){
		first_flgA=0;
		stime = timeGetTime();  //基準時刻
	}else{
        DWORD etime = timeGetTime();           // 現在時間
		DWORD dtime = etime-stime;
		if(dtime>=0.1*1000){//変更できるように
			std::cout<< "IN " << m_human_in << std::endl; //右方向からの人数を表示
			std::cout<< "OUT " << m_human_out << std::endl; //左方向からの人数を表示
			std::cout << "混雑度： " << m_human_wait_num << std::endl;
			m_human_in_numberOut.write();
			m_human_out_numberOut.write();
			m_hcountOut.write();
			m_human_wait_numberOut.write();
			for(int i=0;i<m_NUM_max;i++){
			//fprintf(fp,"%d:%d:%d,%f,%f", t_st->tm_hour,t_st->tm_min,t_st->tm_sec, m_human_in,m_human_out,m_human_wait);
		    //fprintf(fp,"\r");//2-19-8-4kusaka
			//first_flg=1;
			m_human_all = 0;
			//printf("output\n");
			stime=etime;
			}
		}
	}
 
	
	/*fprintf(fp,"%f,%d,%d", count_time, m_human_in , m_human_out );
	fprintf(fp,"\r");*/
	//m_hcountOut.write();

	/*m_human_in_number.data.length(1);
	m_human_in_number.data[0]=m_human_in_num;
	//m_human_in_number.data[0]=100;
	//std::cout<< "IN " << m_human_in_number.data[0] << std::endl;
		//time_t timer;
		//struct tm *t_st;
		//time(&timer);
		//t_st = localtime(&timer);

		//char filename[50];

		
		if(first_flg==1){
		first_flg=0;
		//m_human_wait_numberOut.write();	//ＯＵＴＰＯＲＴにデータを渡す
		//printf("output\n");
		stime = timeGetTime();  //基準時刻
	}
	else{
        DWORD etime = timeGetTime();           // 現在時間

		DWORD dtime = etime-stime;
		if(dtime>=0.15*60*1000){  //変更できるように
			//fprintf(fp,"%d:%d:%d,%d,%d,%d", t_st->tm_hour,t_st->tm_min,t_st->tm_sec, m_human_in , m_human_out, m_human_wait);
		    //fprintf(fp,"\r");
			if(pre_human_num  != m_human_in_num){
				m_human_in_numberOut.write();
				//2016.11.04追加
				pre_human_num = m_human_in_num;
			}
			//printf("output\n");
			stime=etime;
		}
	}

	//m_human_in_numberOut.write();*/


	//m_human_wait_number.data.length(1);//20190805コメントアウトkusaka
	m_human_wait_number.data=m_human_wait_num;
	//m_human_wait_number.data[0]=m_human_wait_num;//20190805コメントアウトkusaka
	
	/*time_t timer;
		struct tm *t_st;
		time(&timer);
		t_st = localtime(&timer);

		char filename[50];*/

		
	if(first_flg==1){
		first_flg=0;
		//m_human_wait_numberOut.write();	//ＯＵＴＰＯＲＴにデータを渡す
		//m_human_all = 0;
		//printf("output\n");
		stime = timeGetTime();  //基準時刻
	}
	else{
        DWORD etime = timeGetTime();           // 現在時間
		DWORD dtime = etime-stime;
		if(dtime>=0.1*1000){//変更できるように*//0.2*60*1000
			//std::cout << "混雑度： " << m_human_wait_num << std::endl;
			//m_human_wait_numberOut.write();
			for(int i=0;i<m_NUM_max;i++){
			//fprintf(fp,"%d:%d:%d,%f,%f", t_st->tm_hour,t_st->tm_min,t_st->tm_sec, m_human_in,m_human_out,m_human_wait);
		    //fprintf(fp,"\r");
			first_flg=1;
			m_human_all = 0;
			//printf("output\n");
			stime=etime;
			}
		}
	}
	
	//m_human_wait_numberOut.write();
	

	/*if(m_qHuman->human[1].distance2[1]<1000 ||m_qHuman->human[1].distance2[2]<1000 || m_qHuman->human[1].distance2[3]<1000 || m_qHuman->human[4].distance2[1]<1000 ||m_qHuman->human[5].distance2[1]<1000 || m_qHuman->human[6].distance2[1]<1000)
	{
		m_point.data.x=-2500;
		m_point.data.y=-2500;
		m_pointOut.write();
	}
	else{
		m_point.data.x=100000;
		m_point.data.y=100000;
		m_pointOut.write();
	}*/
	/*if(m_qHuman->human[1].distance2[1]<1000 ||m_qHuman->human[1].distance2[2]<1000 || m_qHuman->human[1].distance2[3]<1000 || m_qHuman->human[4].distance2[1]<1000 ||m_qHuman->human[5].distance2[1]<1000 || m_qHuman->human[6].distance2[1]<1000)
	{
		m_point.data=1;
		m_pointOut.write();
	}
	else{
		m_point.data=0;
		m_pointOut.write();
	}*/

	for(int ii=1;ii<=m_NUM_max;ii++){
		for(int iii=m_LRFdata_min;iii<=m_LRFdata_max;iii++){
		m_data->parson_divide[ii][iii]=0;
		}
		flag_check_start[ii]=0;
		flag_check_end[ii]=0;			
		move_trace[ii][0]=0;
		move_trace[ii][1]=0;
	}
	m_human_wait=0;
}
//人ID振り分け関数
void Human_Tracking::TARGET_TRACKING(struct Humans *m_MAN){

	/*
	 * ここでは複数人の追跡を行うための処理を行っています．LRFから得られる物体データを生データ，人認識後の位置データを人データとします．
	 * まず，LRFの更新によって変動する各生データと各人の人データの距離を算出します．
	 * 次に，それぞれのデータを比較して最短距離の生データと人データの組み合わせを検索します．
	 * 最後に，見つかった組み合わせの人データに生データの値を入れ，更新します．
	 * この作業を人数分行い，複数人を追跡します．
	 *
	 */


	//設定変数
	const int HUMAN_PRIME_ID[NUM_Hmax]={0,2,3,5,7,11,13,17,19,23,29};	 //各素数(IDHUMAN_PRIME_ID[0]は使わないので0を代入)	
	double dist_check1[NUM_Hmax][NUM_Hmax]={0};							 //各人と各生データとの距離記録用
	int hum_comp[NUM_Hmax]={0};											 //マッチング前の生データの場所保存用
	int hum_comp2=0;                                                     //マッチング前の人データの場所保存用
	static bool c_flag[NUM_Hmax]={0};                                    //マッチング後の生データの場所保存用
	static bool c_flag2[NUM_Hmax]={0};                                   //マッチング後の人データの場所保存用
	bool set_flag[NUM_Hmax]={0};                                         //マッチング後の生データの場所保存用
	bool set_flag2[NUM_Hmax]={0};                                        //マッチング後の人データの場所保存用
	bool Tracking_check2[NUM_Hmax][NUM_Hmax]={0};                        //マッチング後の人データ及びの生データの場所保存用
	static int Hum_ID[NUM_Hmax]={1,1,1,1,1,1,1,1,1,1,1};                 //どのIDを使っているか記録用（1：追跡していない．素数：追跡中）
	int HumID=0;

	for(int t=1;t<=m_NUM_max;t++){                                       //追跡している人を判別
		Hum_ID[t]=m_qHuman->human[t].Hum_ID2;
	}	

	//----------------------------------------------------生データと人データマッチング--------------------------------------------------------------
	for(int i=1 ; i<=m_NUM_max ; i++){
		if(Hum_ID[i]!=1){
			for(int sti=1;sti<=m_NUM_max;sti++){
				if(m_qHuman->human[sti].x[1]!=0 && m_qHuman->human[sti].y[1]!=0 ){
					dist_check1[i][sti]=(int)hypot(m_qHuman->human[i].x2[1]-m_qHuman->human[sti].x[1],m_qHuman->human[i].y2[1]-m_qHuman->human[sti].y[1]);	//各人と各生データとの距離を記録			    
				}	
			}
		}
	}

	for(int i=1;i<=m_NUM_max;i++){ //回数
		int dist_hum[NUM_Hmax]={9999,9999,9999,9999,9999,9999,9999,9999,9999,9999,9999};
		for(int t=1;t<=m_NUM_max;t++){ //人
			for(int sti=1;sti<=m_NUM_max;sti++){ //生データ
				if(dist_check1[t][sti]!=0 && set_flag[sti]!=true && set_flag2[t]!=true){
					if(dist_hum[t]>dist_check1[t][sti]){
						dist_hum[t]=dist_check1[t][sti];hum_comp[t]=sti; //各人に対して最短の生データを検索し生データの位置を記録
					} 
					else{}
				}
			}
		}

		double dist_hum2=9999;
		for(int t=1;t<=m_NUM_max;t++){
			if(dist_hum[t]!=0  ){
				if(dist_hum2>dist_hum[t]){dist_hum2=dist_hum[t];hum_comp2=t;} //各人と生データが最小距離の組を検索
			}
		}
		if(dist_hum2!=9999){	
			m_qHuman->human[hum_comp2].xp2[1]=m_qHuman->human[hum_comp[hum_comp2]].x[1];  //マッチング後の人データを更新
			m_qHuman->human[hum_comp2].yp2[1]=m_qHuman->human[hum_comp[hum_comp2]].y[1];  //マッチング後の人データを更新
			m_qHuman->human[hum_comp2].end = m_qHuman->human[hum_comp2].start;            //更新時間を保存（速度算出時に使用）
			m_qHuman->human[hum_comp2].start = clock();                                   
		if(m_qHuman->human[hum_comp2].x3[1]!=m_qHuman->human[hum_comp2].x3[2]&& m_qHuman->human[hum_comp2].x3[1]!=m_qHuman->human[hum_comp[hum_comp2]].x[1]){
			arrayInsert(m_qHuman->human[hum_comp2].x3,10,m_qHuman->human[hum_comp[hum_comp2]].x[1], 1);  //マッチング後の人データを保存
			arrayInsert(m_qHuman->human[hum_comp2].y3,10,m_qHuman->human[hum_comp[hum_comp2]].y[1], 1);  //マッチング後の人データを保存
		}
		set_flag[hum_comp[hum_comp2]]=true;                    //使用中の生データの場所更新
		set_flag2[hum_comp2]=true;                             //使用中の人データの場所更新
		Tracking_check2[hum_comp2][hum_comp[hum_comp2]]=true;  //使用中の生データ, 生データの場所更新
		}
	}
	for(int t=1;t<=m_NUM_max;t++){
			 c_flag[t]=set_flag[t];
		  	 c_flag2[t]=set_flag2[t];
			 Human_check[t]=set_flag2[t];
		 for(int s=1;s<=m_NUM_max;s++){
			 Tracking_check[t][s]=Tracking_check2[t][s];
			 }
	}
	//----------------------------------------------------生データと人データマッチングここまで--------------------------------------------------------------

	//----------------------------------------------------新しい値が入ってきたとき------------------------------------------------------------------------
	//空いているIDに数値を入れいていく
   for(int sti=1 ; sti<=m_NUM_max ; sti++){
	   //コンフィギュレーションパラメーターHuman_setdistanceで設定した値以上の場所で物体を検出した場合，人データの値を更新
		if(m_qHuman->human[sti].y[1]!=0 && hypot(m_qHuman->human[sti].y[1],m_qHuman->human[sti].x[1])>m_Human_set&& hypot(m_qHuman->human[sti].y[1],m_qHuman->human[sti].x[1])<m_environment && c_flag[sti]!=true){
			int t=PrimeNum_Search(Hum_ID,0);//使われていない値を探す(1以外の使われていない素数を探す．)
	        if(Hum_ID[t]==1){
			for( int i2 = 0 ; i2 < max_particles ; i2 ++ ){
				m_qHuman->human[t].xp[i2]=m_qHuman->human[sti].x[1];  //パーティクルのデータを更新
				m_qHuman->human[t].yp[i2]=m_qHuman->human[sti].y[1];  //パーティクルのデータを更新
				}
				m_qHuman->human[t].x2[1]=m_qHuman->human[sti].x[1]+1; //人データを更新
				m_qHuman->human[t].y2[1]=m_qHuman->human[sti].y[1]+1; //人データを更新
				m_qHuman->human[t].x2[0]=m_qHuman->human[sti].x[1];//初期位置x座標保管
				m_qHuman->human[t].y2[0]=m_qHuman->human[sti].y[1];//初期位置y座標保管
				
				arrayInsert(m_qHuman->human[t].x3,10,m_qHuman->human[sti].x[1], 1); //人データを保存
				arrayInsert(m_qHuman->human[t].y3,10,m_qHuman->human[sti].y[1], 1); //人データを保存
				Hum_ID[t]=PrimeNum_Search(Hum_ID,1);//使っていない一番小さなID（素数）を入れる
				c_flag[sti]=true;
				c_flag2[t]=true;
		}
		}
	}
   //----------------------------------------------------新しい値が入ってきたときここまで------------------------------------------------------------------------


	for(int t=1;t<=m_NUM_max;t++){
		m_qHuman->human[t].Hum_ID2=Hum_ID[t];   //追跡している人を更新
		
	}	

}
//仮に計測データを記録する関数
void Human_Tracking::HUMAN_STATE_CALCULATE(struct Humans *m_MAN){

	
	//物体と認識された有効ステップの中間ステップでLRFに対しての距離と角度を算出．
	for(int i=1;i<=m_NUM_max;i++){
		
		G_center_angl[i] = (move_trace[i][1] + move_trace[i][0])/2;
		
		//std::cout << "move_trace1" << i << ":" << move_trace[i][1]<< std::endl;
		//std::cout << "move_trace2" << i << ":" << move_trace[i][0]<< std::endl;

		//theta_other[i] = (G_center_angl[i] - 96) * m_LRFdata_theta / 360 * 2 * M_PI / m_LRF_steps;		//目標の代表方向にあたる角度をラジアンで計算
		theta_other[i] = (G_center_angl[i] - m_LRFdata_mid) * m_LRFdata_theta / 360 * 2 * M_PI / m_LRF_steps;		//目標の代表方向にあたる角度をラジアンで計算

		
		theta_edge1[i] = (move_trace[i][1]- m_LRFdata_mid) * m_LRFdata_theta / 360 * 2 * M_PI / m_LRF_steps;		//nii(2020/01/29):はし1の角度//
		theta_edge2[i] = (move_trace[i][0]- m_LRFdata_mid) * m_LRFdata_theta / 360 * 2 * M_PI / m_LRF_steps;		//nii(2020/01/29):はし2の角度//
		
		//std::cout << "theta_edge1" << i << ":" << 180/M_PI*theta_edge1[i] << std::endl;
		//std::cout << "theta_edge2" << i << ":" << 180/M_PI*theta_edge2[i] << std::endl;

		if(G_center_angl[i] != 0){

			G_dist_other[i] = m_data->mem_data_x[G_center_angl[i]];		//代表方向を示す線の距離を設定
			
			G_dist_edge1[i] = m_data->mem_data_x[move_trace[i][1]];		//nii(2020/01/29):edge1の距離を設定
			G_dist_edge2[i] = m_data->mem_data_x[move_trace[i][0]];		//nii(2020/01/29):edge2の距離を設定
			std::cout << "G_dist_edge1" << i << ":" << G_dist_edge1[i]<< std::endl;
			std::cout << "G_dist_edge2" << i << ":" << G_dist_edge2[i]<< std::endl;

			//std::cout << "dist_other" << i << ":" << G_dist_other[i]<< std::endl;
			//std::cout << "dist_edge1" << i << ":" << G_dist_edge1[i]<< std::endl;
			//std::cout << "dist_edge2" << i << ":" << G_dist_edge2[i]<< std::endl;

			point_x1[i] = G_dist_other[i] * cos(theta_other[i]);	//nii(2020/01/31):x1
			point_y1[i] = G_dist_other[i] * sin(theta_other[i]);	//nii(2020/01/31):y1

			point_x2[i] = G_dist_edge1[i] * cos(theta_edge1[i]);	//nii(2020/01/31):x2
			point_y2[i] = G_dist_edge1[i] * sin(theta_edge1[i]);	//nii(2020/01/31):y2
			point_x3[i] = G_dist_edge2[i] * cos(theta_edge2[i]);	//nii(2020/01/31):x3
			point_y3[i] = G_dist_edge2[i] * sin(theta_edge2[i]);	//nii(2020/01/31):y3
			//std::cout << "point_x2" << i << ":" << point_x2[i]<< std::endl;
			//std::cout << "point_y2" << i << ":" << point_y2[i]<< std::endl;
			//std::cout << "point_x3" << i << ":" << point_x3[i]<< std::endl;
			//std::cout << "point_y3" << i << ":" << point_y3[i]<< std::endl;

			point_p1[i] = (point_x1[i] + point_x2[i]) / 2;	//nii(2020/01/31):x1とx2の中点
			point_q1[i] = (point_y1[i] + point_y2[i]) / 2;	//nii(2020/01/31):y1とy2の中点
			point_p2[i] = (point_x2[i] + point_x3[i]) / 2;	//nii(2020/01/31):x2とx3の中点
			point_q2[i] = (point_y2[i] + point_y3[i]) / 2;	//nii(2020/01/31):y2とy3の中点
			//std::cout << "p1[" << i << "]:" << point_p1[i]<< std::endl;
			//std::cout << "q1[" << i << "]:" << point_q1[i]<< std::endl;
			//std::cout << "p2[" << i << "]:" << point_p2[i]<< std::endl;
			//std::cout << "q2[" << i << "]:" << point_q2[i]<< std::endl;


			alpha1[i] = -1*(point_x1[i] - point_x2[i]) / (point_y1[i] - point_y2[i]);		//nii(2020/01/31):傾き
			alpha2[i] = -1*(point_x2[i] - point_x3[i]) / (point_y2[i] - point_y3[i]);		//nii(2020/01/31):傾き
			//std::cout << "alpha1" << i << ":" << alpha1[i]<< std::endl;
			//std::cout << "alpha2" << i << ":" << alpha2[i]<< std::endl;

			beta1[i] = point_q1[i] - alpha1[i] * point_p1[i];		//nii(2020/01/31):切片
			beta2[i] = point_q2[i] - alpha2[i] * point_p2[i];		//nii(2020/01/31):切片
			//std::cout << "beta1" << i << ":" << beta1[i]<< std::endl;
			//std::cout << "beta2" << i << ":" << beta2[i]<< std::endl;

			human_center_x[i] = (beta1[i] - beta2[i]) / (alpha2[i] - alpha1[i]);	//nii(2020/01/31):人の中心x
			human_center_y[i] = (beta1[i] - beta2[i]) *alpha1[i] / (alpha2[i] - alpha1[i]) + beta1[i];	//nii(2020/01/31):人の中心y
			//std::cout << "human_center_x" << i << ":" << human_center_x[i]<< std::endl;
			//std::cout << "human_center_y" << i << ":" << human_center_y[i]<< std::endl;
			
			m_humanX.data[i] = human_center_x[i];
			m_humanY.data[i] = human_center_y[i];
			//tajima20200914
			m_humanEdge1x.data[i] = point_x2[i];
			m_humanEdge1y.data[i] = point_y2[i];
			m_humanEdge2x.data[i] = point_x3[i];
			m_humanEdge2y.data[i] = point_y3[i];

		}
		else{
			G_dist_other[i] = 0;
			G_dist_edge1[i] = 0;	//nii(2020/01/30)
			G_dist_edge2[i] = 0;	//nii(2020/01/30)
			
		}
		//m_humanXOut.write();
		//m_humanYOut.write();
	}

	for(int i=1 ; i<=m_NUM_max ; i++){		

			//算出した計測データを記録（過去データを保存可能．[1]：最新データ）
			//----------------------------------------------------極座標表示------------------------------------------											
					arrayInsert(m_MAN -> human[i].theta, 100, -theta_other[i]/M_PI*180, 1);										//人の角度方向												
					arrayInsert(m_MAN -> human[i].edgetheta1, 100, -theta_edge1[i]/M_PI*180, 1);										//nii20200916人の角度方向												
					arrayInsert(m_MAN -> human[i].edgetheta2, 100, -theta_edge2[i]/M_PI*180, 1);										//nii20200916人の角度方向												
					//arrayInsert(m_MAN -> human[i].distance, 100, sqrt(pow(human_center_x[i], 2.0) + pow(human_center_y[i], 2.0)), 1);	//nii(2020/01/31):人の半径距離
					//std::cout << "human_dis" << i << ":" << sqrt(pow(human_center_x[i], 2.0) + pow(human_center_y[i], 2.0))<< std::endl;
					arrayInsert(m_MAN -> human[i].distance, 100, G_dist_other[i], 1);							                //人の半径距離
					arrayInsert(m_MAN -> human[i].edgedistance1, 100, G_dist_edge1[i], 1);							                //nii20200916人の半径距離
					arrayInsert(m_MAN -> human[i].edgedistance2, 100, G_dist_edge2[i], 1);							                //nii20200916人の半径距離

			//----------------------------------------------------直行座標（生データ）---------------------------------
					//arrayInsert(m_MAN -> human[i].x,100,human_center_x[i], 1);								//nii(2020/01/31):x座標
					//arrayInsert(m_MAN -> human[i].y,100,human_center_y[i], 1);								//nii(2020/01/31):y座標
					//std::cout << "human_center_x" << i << ":" << human_center_x[i]<< std::endl;
					//std::cout << "human_center_y" << i << ":" << human_center_y[i]<< std::endl;

					arrayInsert(m_MAN -> human[i].x,100,G_dist_other[i]*cos(theta_other[i]), 1);								//x座標
					arrayInsert(m_MAN -> human[i].y,100,G_dist_other[i]*sin(theta_other[i]), 1);								//y座標
					//tajima20200916
					arrayInsert(m_MAN -> human[i].edgex1,100,G_dist_edge1[i]*cos(theta_edge1[i]), 1);								//x座標
					arrayInsert(m_MAN -> human[i].edgey1,100,G_dist_edge1[i]*sin(theta_edge1[i]), 1);								//y座標
					arrayInsert(m_MAN -> human[i].edgex2,100,G_dist_edge2[i]*cos(theta_edge2[i]), 1);								//x座標
					arrayInsert(m_MAN -> human[i].edgey2,100,G_dist_edge2[i]*sin(theta_edge2[i]), 1);								//y座標
					std::cout << "edgex1[" << i << "]:" << G_dist_edge1[i]*cos(theta_edge1[i])<< std::endl;
					std::cout << "edgey1[" << i << "]:" << G_dist_edge1[i]*sin(theta_edge1[i])<< std::endl;
					std::cout << "edgex2[" << i << "]:" << G_dist_edge2[i]*cos(theta_edge2[i])<< std::endl;
					std::cout << "edgey2[" << i << "]:" << G_dist_edge2[i]*sin(theta_edge2[i])<< std::endl;
					//tajima20200914
					m_humanEdge1x.data.length(m_NUM_max);
					for(int i=0;i<m_NUM_max;i++){
						m_humanEdge1x.data[i] = m_qHuman->human[i+1].edgey1[1];
						//m_humanY.data[i] = m_qHuman->human[i+1].x3[1];
						//m_humanY.data[i] = human_center_x[i];		//nii(2020/02/02)
						//std::cout<<"dataEdge1x[" << i << "]:" <<m_humanEdge1x.data[i]<<std::endl;
					}
					m_humanEdge1xOut.write();

					m_humanEdge1y.data.length(m_NUM_max);
					for(int i=0;i<m_NUM_max;i++){
						m_humanEdge1y.data[i] = m_qHuman->human[i+1].edgex1[1];
						//m_humanY.data[i] = m_qHuman->human[i+1].x3[1];
						//m_humanY.data[i] = human_center_x[i];		//nii(2020/02/02)
						//std::cout<<"dataEdge1y[" << i << "]:" <<m_humanEdge1y.data[i]<<std::endl;
					}
					m_humanEdge1yOut.write();

					m_humanEdge2x.data.length(m_NUM_max);
					for(int i=0;i<m_NUM_max;i++){
						m_humanEdge2x.data[i] = m_qHuman->human[i+1].edgey2[1];
						//m_humanY.data[i] = m_qHuman->human[i+1].x3[1];
						//m_humanY.data[i] = human_center_x[i];		//nii(2020/02/02)
						//std::cout<<"dataEdge2x[" << i << "]:" <<m_humanEdge2x.data[i]<<std::endl;
					}
					m_humanEdge2xOut.write();

					m_humanEdge2y.data.length(m_NUM_max);
					for(int i=0;i<m_NUM_max;i++){
						m_humanEdge2y.data[i] = m_qHuman->human[i+1].edgex2[1];
						//m_humanY.data[i] = m_qHuman->human[i+1].x3[1];
						//m_humanY.data[i] = human_center_x[i];		//nii(2020/02/02)
						//std::cout<<"dataEdge2y[" << i << "]:" <<m_humanEdge2y.data[i]<<std::endl;
					}
					m_humanEdge2yOut.write();


	}
	

}
//移動距離計測および人数カウント関数
void Human_Tracking::HUMAN_MIGRATION_LENGTH(int i,struct Humans *m_MAN){

	if(m_MAN->human[i].x3[1]!=100000){

	//arrayInsert(m_MAN->human[i].distance2,100,hypot(human_center_y[i],human_center_x[i]),1);		 //nii(2020/02/01):LRFから各人の距離を算出
	//arrayInsert(m_MAN->human[i].theta2,100,-atan2(m_MAN->human[i].y2[1],m_MAN->human[i].x2[1])/M_PI*180,1);  //nii(2020/02/01):LRFから各人の角度を算出
	arrayInsert(m_MAN->human[i].distance2,100,hypot(m_MAN->human[i].y2[1],m_MAN->human[i].x2[1]),1);		 //LRFから各人の距離を算出
	arrayInsert(m_MAN->human[i].theta2,100,-atan2(m_MAN->human[i].y2[1],m_MAN->human[i].x2[1])/M_PI*180,1);  //LRFから各人の角度を算出
	
	//tajima20200916
	//arrayInsert(m_MAN->human[i].edgedistance1,100,hypot(m_MAN->human[i].edgey1[1],m_MAN->human[i].edgex1[1]),1);		 //LRFから各人の距離を算出
	//arrayInsert(m_MAN->human[i].edgedistance2,100,hypot(m_MAN->human[i].edgey2[1],m_MAN->human[i].edgex2[1]),1);		 //LRFから各人の距離を算出
	//arrayInsert(m_MAN->human[i].edgetheta1,100,-atan2(m_MAN->human[i].edgey1[1],m_MAN->human[i].edgex1[1])/M_PI*180,1);  //LRFから各人の角度を算出
	//arrayInsert(m_MAN->human[i].edgetheta2,100,-atan2(m_MAN->human[i].edgey2[1],m_MAN->human[i].edgex2[1])/M_PI*180,1);  //LRFから各人の角度を算出
	//std::cout << "edgedistance1" << i << ":" << m_MAN->human[i].edgedistance1<< std::endl;
	//std::cout << "edgedistance2" << i << ":" << m_MAN->human[i].edgedistance2<< std::endl;

	}
        //追跡処理後の直行座標データから速さと向きを計算
	if(m_MAN->human[i].x3[1]!=100000&&m_MAN->human[i].x3[1]<m_environment&&m_MAN->human[i].y3[1]<m_environment){
		if(hypot((m_MAN->human[i].x3[1]-m_MAN->human[i].x3[2])/(((double)m_MAN->human[i].start-(double)m_MAN->human[i].end)/1000),(m_MAN->human[i].y3[1]-m_MAN->human[i].y3[2])/(((double)m_MAN->human[i].start-(double)m_MAN->human[i].end)/1000))<2000)
			arrayInsert(m_MAN->human[i].velocity,10,hypot((m_MAN->human[i].x3[1]-m_MAN->human[i].x3[2])/(((double)m_MAN->human[i].start-(double)m_MAN->human[i].end)/1000),(m_MAN->human[i].y3[1]-m_MAN->human[i].y3[2])/(((double)m_MAN->human[i].start-(double)m_MAN->human[i].end)/1000)),1);	
		else{}
		if(m_MAN->human[i].velocity[1]>300&&m_MAN->human[i].velocity[1]<2000){
			arrayInsert(m_MAN->human[i].vector,10,-atan2((m_MAN->human[i].y3[1]-m_MAN->human[i].y3[2]),(m_MAN->human[i].x3[1]-m_MAN->human[i].x3[2]))/M_PI*180,1);
		}

		if(m_MAN->human[i].x3[1]!=100000&&m_MAN->human[i].x3[1]!=0&&m_MAN->human[i].x3[1]<m_environment&&m_MAN->human[i].y3[1]<m_environment){
		distance_xy[i][1]=m_MAN->human[i].x3[1]-m_MAN->human[i].x3[2];//x方向総移動量
		distance_xy[i][2]=m_MAN->human[i].y3[1]-m_MAN->human[i].y3[2];//x方向総移動量
		distance_S[i]=sqrt(distance_xy[i][1]*distance_xy[i][1]+distance_xy[i][2]*distance_xy[i][2]);//0.1秒間の移動量
		if(m_MAN->human[i].velocity[1]>300&&m_MAN->human[i].velocity[1]<2000&&m_MAN->human[i].velocity[1]!=m_MAN->human[i].velocity[2]){
		distance_S2[i]+=distance_S[i];
		}

		//std::cout << "Data is " << distance_S2[1]<< std::endl;
		//distance_xy2[i][1]=m_MAN->human[i].x2[1]-m_MAN->human[i].x2[100];//0.5秒間の移動量
		//distance_xy2[i][2]=m_MAN->human[i].y2[1]-m_MAN->human[i].y2[100];//0.5秒間の移動量
		//distance_S2[i]=hypot(distance_xy2[i][1],distance_xy2[i][2]);
		//distance_xy3[i][1]=m_MAN->human[i].x2[1]-m_MAN->human[i].x2[2];//0.1秒間の移動量
		//distance_xy3[i][2]=m_MAN->human[i].y2[1]-m_MAN->human[i].y2[2];//0.1秒間の移動量
		//distance_S3[i]=hypot(distance_xy3[i][1],distance_xy3[i][2]);//総移動量
		}
	}				
	//人数カウント部分，コンフィギュレーションパラメーターCount_modeを変更することで入退室の向きを変更可能．
	if(m_Count_mode=="IN,OUT"){
		if(m_MAN->human[i].distance2[1]>m_Count_area && m_MAN->human[i].distance2[1]<m_environment && m_qHuman->human[i].theta2[1]>0 && m_MAN->human[i].Msg_Lv[2]!=3){
			m_MAN->human[i].Msg_Lv[2]=1;
		}else if(m_MAN->human[i].distance2[1]>m_Count_area && m_MAN->human[i].distance2[1]<m_environment && m_qHuman->human[i].theta2[1]<0 && m_MAN->human[i].Msg_Lv[2]!=3){
			m_MAN->human[i].Msg_Lv[2]=2;
		}

		/*if(m_MAN->human[i].distance2[1]>m_Count_area && m_MAN->human[i].distance2[1]<m_environment && (m_qHuman->human[i].theta2[1]>0 || m_qHuman->human[i].theta2[1]<0) && m_MAN->human[i].Msg_Lv[2]!=3){
			m_MAN->human[i].Msg_Lv[2]=4;
		}*///20190722Kusakaコメントアウト
		else{}

		if(m_MAN->human[i].Msg_Lv[2] == 1 && m_MAN->human[i].distance2[1]>m_Count_area-500 && m_MAN->human[i].distance2[1]<m_environment && m_qHuman->human[i].theta2[1]<0){
			m_human_in++;
			//std::cout << "m_human_in人数： " << m_human_in << std::endl;
			m_MAN->human[i].Msg_Lv[2]=3;
			/*if(m_human_in==100){
				m_human_in_num=100;   //100人目
			}
			else if(m_human_in==200){
				m_human_in_num=200;   //200人目
			}
			else if(m_human_in==300){
				m_human_in_num=300;   //300人目
			}*/
			//else{}
					
		}else if(m_MAN->human[i].Msg_Lv[2]==2 && m_MAN->human[i].distance2[1]>m_Count_area-500 && m_MAN->human[i].distance2[1]<m_environment && m_qHuman->human[i].theta2[1]>0){
			m_human_out++;
			//std::cout << "m_human_out人数： " << m_human_out << std::endl;
			m_MAN->human[i].Msg_Lv[2]=3;
		}

		if(m_MAN->human[i].Msg_Lv[2] == 4 && m_MAN->human[i].distance2[1]>m_Count_area-500 && m_MAN->human[i].distance2[1]<m_environment && (m_qHuman->human[i].theta2[1]<0  || m_qHuman->human[i].theta2[1]>0)){
			m_human_all++;
			m_MAN->human[i].Msg_Lv[2]=3;
			//std::cout << "人数： " << m_human_all << std::endl;
		}
		//else{}
		
		
	}
	else if (m_Count_mode=="OUT,IN"){
		if(m_MAN->human[i].distance2[1]>m_Count_area && m_MAN->human[i].distance2[1]<m_environment && m_qHuman->human[i].theta2[1]<0 && m_MAN->human[i].Msg_Lv[2]!=3){
			m_MAN->human[i].Msg_Lv[2]=1;
		}
		else if(m_MAN->human[i].distance2[1]>m_Count_area && m_MAN->human[i].distance2[1]<m_environment && m_qHuman->human[i].theta2[1]>0 && m_MAN->human[i].Msg_Lv[2]!=3){
			m_MAN->human[i].Msg_Lv[2]=2;
		}
		else{}

		if(m_MAN->human[i].Msg_Lv[2] == 1 && m_MAN->human[i].distance2[1]>m_Count_area-500 && m_MAN->human[i].distance2[1]<m_environment && m_qHuman->human[i].theta2[1]>0){
			m_human_in++;
			//std::cout << "in： " << m_human_in << std::endl;
			m_MAN->human[i].Msg_Lv[2]=3;
				/*if(m_human_in==1){
				m_human_in_num=100;   //100人目
			}
			else if(m_human_in==2){
				m_human_in_num=200;   //200人目
			}
			else if(m_human_in==3){
				m_human_in_num=300;   //300人目
			}*/
			//else{}
		}
					
		else if(m_MAN->human[i].Msg_Lv[2]==2 && m_MAN->human[i].distance2[1]>m_Count_area-500 && m_MAN->human[i].distance2[1]<m_environment && m_qHuman->human[i].theta2[1]<0){
			m_human_out++;
			//std::cout << "out： " << m_human_out << std::endl;
			m_MAN->human[i].Msg_Lv[2]=3;
		}

		if(m_MAN->human[i].Msg_Lv[2] == 1 && m_MAN->human[i].Msg_Lv[2]==2 && m_MAN->human[i].distance2[1]>m_Count_area-500 && m_MAN->human[i].distance2[1]<m_environment && m_qHuman->human[i].theta2[1]<0   && m_qHuman->human[i].theta2[1]>0){
			m_human_all++;
			//m_MAN->human[i].Msg_Lv[2]=3;
			//std::cout << "人数： " << m_human_all << std::endl;
		}
		else{}
	}
	else{
		if(m_MAN->human[i].distance2[1]>m_Count_area && m_MAN->human[i].distance2[1]<m_environment && m_qHuman->human[i].theta2[1]>0 && m_MAN->human[i].Msg_Lv[2]!=3){
			m_MAN->human[i].Msg_Lv[2]=1;
		}
		else if(m_MAN->human[i].distance2[1]>m_Count_area && m_MAN->human[i].distance2[1]<m_environment && m_qHuman->human[i].theta2[1]<0 && m_MAN->human[i].Msg_Lv[2]!=3){
			m_MAN->human[i].Msg_Lv[2]=2;
		}
		else{}

		if(m_MAN->human[i].Msg_Lv[2] == 1 && m_MAN->human[i].distance2[1]>m_Count_area-500 && m_MAN->human[i].distance2[1]<m_environment && m_qHuman->human[i].theta2[1]<0){
			m_human_in++;
			//std::cout << "in： " << m_human_in << std::endl;
			/*m_MAN->human[i].Msg_Lv[2]=3;
				if(m_human_in==100){
				m_human_in_num=100;   //100人目
			}
			else if(m_human_in==200){
				m_human_in_num=200;   //200人目
			}
			else if(m_human_in==300){
				m_human_in_num=300;   //300人目
			}*/
			//else{}
		}
		
					
		else if(m_MAN->human[i].Msg_Lv[2]==2 && m_MAN->human[i].distance2[1]>m_Count_area-500 && m_MAN->human[i].distance2[1]<m_environment && m_qHuman->human[i].theta2[1]>0){
			m_human_out++;
			//std::cout << "out： " << m_human_out << std::endl;
			m_MAN->human[i].Msg_Lv[2]=3;
		}

		if(m_MAN->human[i].Msg_Lv[2] == 1 && m_MAN->human[i].Msg_Lv[2]==2 && m_MAN->human[i].distance2[1]>m_Count_area-500 && m_MAN->human[i].distance2[1]<m_environment && m_qHuman->human[i].theta2[1]<0   && m_qHuman->human[i].theta2[1]>0){
			m_human_all++;
			//m_MAN->human[i].Msg_Lv[2]=3;
			std::cout << "人数： " << m_human_all << std::endl;
		}

		else{}
	}
	//if(m_qHuman->human[i].Hum_ID2!=1)m_human_wait++;  //滞在人数をカウント

	if(m_qHuman->human[i].distance2[1] > 0 &&m_qHuman->human[i].distance2[1] < 3000) m_human_wait++;  //滞在人数をカウント
	//m_human_wait = 5;
	//std::cout << "滞在人数： " << m_human_wait << std::endl;
	if(0<=m_human_wait && m_human_wait<=3){   //混雑度の定義
		m_human_wait_num=1;        //混雑度　小
		//std::cout << "混雑度： " << m_human_wait_num << std::endl;
	}
	else if(4<=m_human_wait && m_human_wait<=7){
		m_human_wait_num=2;        //混雑度　中
		//std::cout << "混雑度： " << m_human_wait_num << std::endl;
	}
	else {
		m_human_wait_num=3;        //混雑度　大
		//std::cout << "混雑度： " << m_human_wait_num << std::endl;
	}

	
}
//入退室後や長時間残留する人データの残留時間をカウントする関数
void Human_Tracking::HUMAN_DELETE_COUNT(int i,struct Humans *m_MAN){

	if(m_MAN->human[i].velocity[1]==m_MAN->human[i].velocity[2]&& m_MAN->human[i].distance2[1]<m_environment)m_MAN->human[i].hum_count[1]++;
	if(m_MAN->human[i].distance2[1]>2500 && m_MAN->human[i].distance2[1]<m_environment && m_MAN->human[i].velocity[1]<180)m_MAN->human[i].hum_count[1]++;

	else if(m_MAN->human[i].distance2[1]<2500)m_MAN->human[i].hum_count[1]=0; 
	else if(m_MAN->human[i].velocity[1]>180 && m_MAN->human[i].velocity[1]!=m_MAN->human[i].velocity[2])m_MAN->human[i].hum_count[1]=0;
	else{}
	if(m_MAN->human[i].distance2[1]<2500 && m_MAN->human[i].velocity[1]<180 )m_MAN->human[i].hum_count[2]++;
	else if(m_MAN->human[i].velocity[1]>180)m_MAN->human[i].hum_count[2]=0;
	else{}
	if(m_MAN->human[i].distance2[1]>m_environment)m_MAN->human[i].hum_count[3]++;
	if(m_MAN->human[i].velocity[1]==m_MAN->human[i].velocity[2]&& m_MAN->human[i].distance2[1]<m_environment)m_MAN->human[i].hum_count[4]++;
	if(m_MAN->human[i].x3[1]<0 && m_MAN->human[i].velocity[1]<180)m_MAN->human[i].hum_count[4]++;
	else if(m_MAN->human[i].x3[1]>0)m_MAN->human[i].hum_count[4]=0; 
	else if(m_MAN->human[i].velocity[1]>180 && m_MAN->human[i].velocity[1]!=m_MAN->human[i].velocity[2])m_MAN->human[i].hum_count[4]=0;
	else{}
	if(m_MAN->human[i].x2[1]<0)m_MAN->human[i].hum_count[5]++; 
	else if(m_MAN->human[i].x2[1]>0)m_MAN->human[i].hum_count[5]=0;
	if(distance_S2[i]>5000&&m_MAN->human[i].Msg_Lv[2]!=0&&m_MAN->human[i].distance2[1]>3500)m_MAN->human[i].hum_count[6]++; 
	else if(m_MAN->human[i].distance2[1]<4000)m_MAN->human[i].hum_count[6]=0;	
}
//人データを削除する関数
void Human_Tracking::HUMAN_DELETE(struct Humans *m_MAN){

	for(int i=1 ; i<=m_NUM_max ; i++){
		if(m_MAN->human[i].hum_count[1]>=10 ||  m_MAN->human[i].hum_count[3]>10 || m_MAN->human[i].hum_count[4]>20 ||m_MAN->human[i].hum_count[5]>10 ||m_MAN->human[i].hum_count[6]>10 /*||abs(m_MAN->human[i].distance2[1]-m_MAN->human[i].distance2[2])<=0.06*/){//m_MAN->human[i].hum_count[2]>2000 ||){
		//if(m_MAN->human[i].hum_count[6]>0){
		for(int s2=1;s2<=m_NUM_max;s2++){
				m_MAN->human[i].hum_count[s2]=0;
				distance_S2[i]=0;
				flag_check_start[i]=0;
				flag_check_end[i]=0;
			}
		for(int i2=0;i2<max_particles;i2++){
				m_MAN->human[i].weight[i2]=0;
				m_MAN->human[i].xp[i2]=100000;
				m_MAN->human[i].yp[i2]=100000;
				m_MAN->human[i].x2[i2]=100000;
				m_MAN->human[i].y2[i2]=100000;
				m_MAN->human[i].edgex1[i2]=100000;
				m_MAN->human[i].edgex2[i2]=100000;
				m_MAN->human[i].edgey1[i2]=100000;
				m_MAN->human[i].edgey2[i2]=100000;
				m_MAN->human[i].y3[i2]=100000;
				m_MAN->human[i].distance2[i2]=100000;
				m_MAN->human[i].edgedistance1[i2]=100000;
				m_MAN->human[i].edgedistance2[i2]=100000;
				m_MAN->human[i].vector[i2]=0;
				m_MAN->human[i].velocity[i2]=0;
			}
			for(int count = m_LRFdata_min;count <= m_LRFdata_max;count++){
				m_data->m_different_search[count]=0;
			}
			m_MAN->human[i].Hum_ID2=1;
			m_MAN->human[i].Msg_Lv[1]=0;
			m_MAN->human[i].Msg_Lv[2]=0;
			m_MAN->human[i].Msg_Lv[3]=0;
			m_MAN->human[i].theta[0]=0;
			m_MAN->human[i].theta2[0]=0;
			m_MAN->human[i].edgetheta1[0]=0;
			m_MAN->human[i].edgetheta2[0]=0;
			m_MAN->human[i].distance[0]=0;
			//m_MAN->human[i].edgedistance1[0]=0;
			//m_MAN->human[i].edgedistance2[0]=0;
		}
	}
}
//計測周期ごとのデータを残す関数/一番古いデータを消して新しい数値に更新する関数（[1]:最新データ）
void Human_Tracking::arrayInsert(double data[], int n, double newdata, int k)
{
    int i ;
	
    for (i = n; i > k; i--) {
        data[i] = data[i-1];
		    }
    data[k] = newdata;
	
}
//未使用ID(素数)探索関数
int Human_Tracking::PrimeNum_Search(int HUMAN_ID[],int MODE){
		//const int N=7;
		int ANS=1;
		const int SEARCH_NUM[NUM_Hmax]={0,2,3,5,7,11,13,17,19,23,29};//検索する素数[0]は検索しない
		int Discovery_NUM=0;//発見した数を入れる関数
			//全てのIDを掛ける
			for(int i=1;i<=NUM_Hmax;i++){
				if(ANS*HUMAN_ID[i]!=0)ANS*=HUMAN_ID[i];	
			}
			//素数検索
			for(int i=1;i<=NUM_Hmax;i++){
				if(ANS%SEARCH_NUM[i]!=0){
					if(MODE==0)Discovery_NUM=i;//存在しない素数で一番小さい値を記録（同時に検索することも可能）
					else Discovery_NUM=SEARCH_NUM[i];//存在しない素数で一番小さい値IDを記録（同時に検索することも可能）
					break;
				}
			}
 return Discovery_NUM;//使われていないIDで一番小さいのを返す
}
// ログ記録
//ログ入力部（ファイルに直接書き込み-汎用版）
void Human_Tracking::Logsave_Input(int ID,CString File_Name,float log1,float log2,float log3,float log4,float log5,float log6,float log7,float log8)
{
			static int logmode[10];
			CStdioFile fout;
			CString   wstr;
			SYSTEMTIME systime;
			GetLocalTime(&systime);//現在時刻取得
			CString str;
			CString str2;
			CString str3;
			str2.Format(_T("%u"),systime.wMilliseconds);
			if(_tcsclen(str2)==1){str3="00"+str2;}
			else if(_tcsclen(str2)==2){str3="0"+str2;}
			else {str3=str2;}	
			str.Format(_T("%T:%u:%u"),systime.wHour, systime.wMinute, systime.wSecond);
			str=str+"."+str3;
			CString s=str;//現在時刻
			
		if(logmode[ID]==0){//1行目
				fout.Open(File_Name,CFile::modeWrite | CFile::modeCreate| CFile::typeBinary);
				wstr.Format(_T("現在時刻,経過時間,log1,log2,log3,log4,log5,log6,log7,log8\n%s,%f,%f,%f,%f,%f,%f,%f,%f,%f\n"),s,count_time,log1,log2,log3,log4,log5,log6,log7,log8);
				fout.SeekToEnd();
				fout.WriteString(wstr);
				logmode[ID]++;
				}
		else if(logmode[ID]!=0){//2行目以降
				fout.Open(File_Name,CFile::modeWrite| CFile::typeBinary );
				wstr.Format(_T("%s,%f,%f,%f,%f,%f,%f,%f,%f,%f\n"),s,count_time,log1,log2,log3,log4,log5,log6,log7,log8);
				fout.SeekToEnd();
				fout.WriteString(wstr);
				logmode[ID]++;
				}



}
void Human_Tracking::Logsave_Input2(int ID,CString File_Name)
{
			static int logmode2[10];
			CStdioFile fout;
			CString   wstr;
			SYSTEMTIME systime;
			GetLocalTime(&systime);//現在時刻取得
			CString str;
			//str.Format(_T("%u:%u:%u.%u"),systime.wHour, systime.wMinute, systime.wSecond,systime.wMilliseconds);
			CString write[1100];
			CString write2;
			CString str2;
			CString str3;
			str2.Format(_T("%u"),systime.wMilliseconds);
			if(_tcsclen(str2)==1){str3="00"+str2;}
			else if(_tcsclen(str2)==2){str3="0"+str2;}
			else {str3=str2;}	
			str.Format(_T("%u:%u:%u"),systime.wHour, systime.wMinute, systime.wSecond);
			str=str+"."+str3;
			CString s=str;//現在時刻
			
			timenow=str;


			for(int count = m_LRFdata_min;count <= m_LRFdata_max;count++){
				write[count].Format(_T("%lf"), m_data->mem_data_x4[count]);
			}	
			for(int count = m_LRFdata_min;count <= m_LRFdata_max;count++){
				if(count==m_LRFdata_max)write2+=write[count];
				else write2+=write[count];write2+=_T(",");
				//if(count==m_LRFdata_max){write+= _T(count);}
				//else{ write+=count;write+=_T(",");}
			}

		if(logmode2[ID]==0){//1行目
				fout.Open(File_Name,CFile::modeWrite | CFile::modeCreate| CFile::typeBinary);
				wstr.Format(_T("現在時刻,取得データ\n%s,%s\n"),s,write2);
				fout.SeekToEnd();
				fout.WriteString(wstr);
				logmode2[ID]++;
				}
		else if(logmode2[ID]!=0){//2行目以降
				fout.Open(File_Name,CFile::modeWrite| CFile::typeBinary );
				wstr.Format(_T("%s,%s\n"),s,write2);
				fout.SeekToEnd();
				fout.WriteString(wstr);
				logmode2[ID]++;
				}
}
//時間取得
void Human_Tracking::TIME_SET(){
/*	time_t now = time(NULL);
    struct tm *pnow = localtime(&now);
	//char buff[128]="";
	sprintf(time_total,"%d:%d:%d",pnow->tm_year+1900,pnow->tm_mon + 1,pnow->tm_mday);
	time_h=pnow->tm_hour;
	time_htmp=pnow->tm_hour;
	time_m=pnow->tm_min;
	time_s=pnow->tm_sec;*/
	static bool time_flag=0;
	CString strh;
	CString strm;
	CString strs;
	CString strms;
	SYSTEMTIME systime;
	GetLocalTime(&systime);//現在時刻取得

	strh.Format(_T("%u"),systime.wHour);
	strm.Format(_T("%u"),systime.wMinute);
	strs.Format(_T("%u"),systime.wSecond);
	strms.Format(_T("%u"),systime.wMilliseconds);
	time_h=_ttoi(strh);
	time_m=_ttoi(strm);
	time_s=_ttoi(strs);
	time_ms=_ttoi(strms);
	if(time_flag==0){
	time_htmp=time_h*3600+time_m*60+time_s+time_ms*0.001;
	time_flag=1;
	}
	count_time = time_h*3600+time_m*60+time_s+time_ms*0.001;
	count_time -= time_htmp;
	count_time *=100;              //小数点第三位を四捨五入
	count_time +=0.5;
	count_time = (int)count_time;
	count_time /=100;
	//std::cout << "Data is " <<count_time<< std::endl;
}
//CSV読み込み　ＯＮにするときはonActivated内の関数を使う。
void Human_Tracking::CSV_CHEAK(){
	CStdioFile fout1;
	CStdioFile fout2;
	CString   rstr1="";
	CString   rstr2="";
	int ssss=0;

	fout1.Open("log_data1-3.csv",CFile::modeRead | CFile::typeBinary);			
	while (fout1.ReadString(rstr1))
    {
		rstr1_2[ssss]=rstr1;
        //TRACE(rstr1);//20190804kusaka
		//if(ssss>1000)break;//20190804kusaka
		ssss++;
    }
    fout1.Close();
	fout2.Open("log_data4-6.csv",CFile::modeRead | CFile::typeBinary);			
	while (fout2.ReadString(rstr2))
    {
		rstr2_2[ssss]=rstr2;
        //TRACE(rstr2[1]);//20190804kusaka
		//if(ssss>1000)break;//20190804kusaka
		//TRACE(rstr2);
		//if(ssss>1000)break;
		ssss++;
    }
    fout2.Close();
}
void Human_Tracking::token(){
	static int ss3=0;
    CAtlString resToken;
	//CAtlString resToken2;
    int curPos= 0;
	int curPos2= 0;
	int i=0;
	int i2=0;
    resToken= rstr1_2[ss3].Tokenize(",",curPos);
    while (resToken != "")
    {
        humandata1_1[i]=resToken;
        resToken= rstr1_2[ss3].Tokenize(",",curPos);
	//	humandata1_1[i]=resToken;
	//	printf("%s\n", humandata1_1[i]);
		//std::cout << "Data is " <<humandata1_1[0]<< std::endl;
		i++;
    }
	resToken= rstr2_2[ss3].Tokenize(",",curPos2);
    while (resToken != "")
    {
        humandata2_2[i]=resToken;
        resToken= rstr2_2[ss3].Tokenize(",",curPos2);
		//humandata[i]=resToken;
		//printf("%s\n", humandata[i]);
		i2++;
    }
	ss3++;
}

//パーティクルフィルタによるロックオン処理
void Human_Tracking::resample( int i, struct Humans *m_MAN,struct Humans *m_MAN2)
{
    // 累積重みの計算
	double all_weight[max_particles]={0.0};
	all_weight[0]=m_MAN->human[i].weight[0];
	for(int i2=1;i2<max_particles;i2++)
	{
		all_weight[i2]=all_weight[i2-1]+m_MAN->human[i].weight[i2];
		
	}
    // 重みを基準にパーティクルをリサンプリングして重みを1.0に
	for(int i2=0;i2<max_particles;i2++)
	{
		m_MAN2->human[i].weight[i2] = m_MAN->human[i].weight[i2];
		m_MAN2->human[i].xp[i2] = m_MAN->human[i].xp[i2];
		m_MAN2->human[i].yp[i2] = m_MAN->human[i].yp[i2];
	}
    for( int i2 = 0 ; i2 < max_particles; i2 ++ )
    {
		double rate=(double)(rand()%100)/100;
		if(rate<cut_rate)rate+=(1.0-cut_rate);
        const double weight = rate * all_weight[max_particles-1];
        size_t n = 0;
        while( all_weight[++n] < weight );
		m_MAN->human[i].weight[i2]=m_MAN2->human[i].weight[n];
		m_MAN->human[i].xp[i2]=m_MAN2->human[i].xp[n];
		m_MAN->human[i].yp[i2]=m_MAN2->human[i].yp[n];
        m_MAN->human[i].weight[i2] = 1.0;
    }



}
void Human_Tracking::predict(int i, struct Humans *m_MAN)
{
	double variance =0;
	int sum =0;
	//この値を移動距離の積算と共に増大させよう
	if(m_MAN->human[i].Hum_ID2!=1){
		for(int s=1;s<=m_NUM_max;s++){
			sum += Tracking_check[i][s];
		}
		if(sum==0){
			// 位置の予測
			for( size_t i2 = 0 ; i2 < max_particles ; i2 ++ )
			{
				if(m_MAN->human[i].velocity[1]>300 && m_MAN->human[i].distance2[1]<m_Human_TrackingData){
					double rate=0.0;
					rate=(double)(rand()%100-20);
					double vx2= m_MAN->human[i].velocity[1]*(((double)m_MAN->human[i].start-(double)m_MAN->human[i].end)/1000)*cos(m_qHuman->human[i].vector[1]*M_PI/180.0)/20.0*rate;
					rate=(double)(rand()%100-20);
					double vx3= -m_MAN->human[i].velocity[1]*(((double)m_MAN->human[i].start-(double)m_MAN->human[i].end)/1000)*sin(m_qHuman->human[i].vector[1]*M_PI/180.0)/20.0*rate;			
					m_MAN->human[i].xp[i2] += vx2;
					m_MAN->human[i].yp[i2] += vx3;
				}
				else {
					variance = 4.0;	//nii(2020/02/02):4.0
					double rate=0.0;
					rate=(double)(rand()%200-100);
					double vx2= variance*rate;
					rate=(double)(rand()%200-100);
					double vx3= variance*rate;
					m_MAN->human[i].xp[i2] += vx2;
					m_MAN->human[i].yp[i2] += vx3;

				}
				
			}
		}
		else if(sum==1){
				if(m_MAN->human[i].velocity[1]<200){
					variance = 1.0;
				}
				else if(200<m_MAN->human[i].velocity[1] ){
					variance = 2.0;
				}
				else{}
				if(m_MAN->human[i].velocity[1]>700||m_MAN->human[i].velocity[1]==m_MAN->human[i].velocity[2]){
					variance = 4.0;
				}
				else{}

    
					// 位置の予測
				for( size_t i2 = 0 ; i2 < max_particles ; i2 ++ )
				{
					double rate=0.0;
					rate=(double)(rand()%200-100);
					double vx2= variance*rate;
					rate=(double)(rand()%200-100);
					double vx3= variance*rate;
					m_MAN->human[i].xp[i2] += vx2;
					m_MAN->human[i].yp[i2] += vx3;	
				}
		}
		//else{}
	}
	//else{}

}
void Human_Tracking::weight( int i, struct Humans *m_MAN)
{
    // 尤度に従いパーティクルの重みを決定する
	double max_l_dist=0.0,heading=9999;
	double lrf_weight[max_particles];


	for(int i2=0;i2<max_particles;i2++)
	{
		lrf_weight[i2]=9999;		
		double dist=0.0;
		double delta_x=m_qHuman->human[i].xp2[1]-m_MAN->human[i].xp[i2];
		double delta_y=m_qHuman->human[i].yp2[1]-m_MAN->human[i].yp[i2];
		dist=hypot(delta_x,delta_y);
		if(lrf_weight[i2]>dist)lrf_weight[i2]=dist;		
	}
	
	for(int i2=0;i2<max_particles;i2++)
	{
		if(lrf_weight[i2]>max_l_dist) max_l_dist=lrf_weight[i2];
	}
	for(int i2=0;i2<max_particles;i2++)
	{
		m_MAN->human[i].weight[i2]=1-lrf_weight[i2]/max_l_dist;
		
	}

}
void Human_Tracking::measure(int i, struct Humans *m_MAN)
{
    double  x = 0.0;
    double  y = 0.0;
    double  weight = 0.0;

    // 重み和
    for( size_t i2 = 0 ; i2 < max_particles ; i2 ++ )
    {
        x       += m_MAN->human[i].xp[i2]         * m_MAN->human[i].weight[i2];
        y       += m_MAN->human[i].yp[i2]         * m_MAN->human[i].weight[i2];
        weight  += m_MAN->human[i].weight[i2];
    }
    // 正規化
	if(weight!=0){
	arrayInsert(m_qHuman->human[i].x2,100,static_cast< int >( x / weight ), 1);
	arrayInsert(m_qHuman->human[i].y2,100,static_cast< int >( y / weight ), 1);
	}
	for(int i2=0;i2<max_particles;i2++){
		tmp_x[i][i2]=m_MAN->human[i].xp[i2];
		tmp_y[i][i2]=m_MAN->human[i].yp[i2];
		}
}



/*
RTC::ReturnCode_t Human_Tracking::onAborting(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Human_Tracking::onError(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Human_Tracking::onReset(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Human_Tracking::onStateUpdate(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Human_Tracking::onRateChanged(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/


bool Human_Tracking::log_open(){
	if(open_check == false){
		//時間情報
		time_t timer;
		struct tm *t_st;
		time(&timer);
		t_st = localtime(&timer);

		char filename[50];
		sprintf(filename, "%02d%02d_%02d%02d%02d_log.csv", t_st->tm_mon+1, t_st->tm_mday,t_st->tm_hour,t_st->tm_min,t_st->tm_sec);

		if(( fp = fopen(filename, "w")) == NULL){
			printf("file open error\n");
			return false;
		}
		fprintf(fp, "Time, IN, OUT,crowd");
		fprintf(fp, "\r\n");

		open_check = true;
	}
}

bool Human_Tracking::log_close(){
	if(open_check == true){
		fclose(fp);
		open_check = false;
	}
	return true;
}
extern "C"
{
 
  void Human_TrackingInit(RTC::Manager* manager)
  {
    coil::Properties profile(human_tracking_spec);
    manager->registerFactory(profile,
                             RTC::Create<Human_Tracking>,
                             RTC::Delete<Human_Tracking>);
  }
  
};


