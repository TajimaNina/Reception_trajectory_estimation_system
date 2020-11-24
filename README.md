# 「受付など通過歩行時の人数推定RTCの開発」

## 概要
- LRFで複数人の軌跡を測定する
- 軌跡をガウス過程回帰を用いることで予測する

## 特徴
- 複数人の軌跡の補正を行う
- 人の座標と人数を入力するだけで容易にトラッキング精度が向上する

## コンポーネント

### 開発したコンポーネント
- ReceptionCommander RTC
  - コマンドを生成し，logger RTCとgaussian_process_multi RTCの起動と停止を行う．
- logger RTC
  - HumanTracking RTCで取得した人の座標を記録する．
- gaussian_process_multi RTC
  - 人の座標と人数を入力すると，ガウス過程回帰で人の軌跡を予測する

### 既存コンポーネント
- URG RTC
  - LRFの値を取得する
- HumanTracking RTC
  - 人を検出し，人の中心座標と端点を取得する

