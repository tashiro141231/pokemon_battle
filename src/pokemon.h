/**
 * @file pokemon.h
 * @brief トレーナーが持つポケモンを管理するクラスの定義
 * @author Yosuke Tashiro
 * @date 2017/12/11
 * @detail
 */

#include <ros/ros.h>
#include <vector>
#include <string>

//ポケモンの状態テーブル
enum StatusAilment{
  NORMAL = 0,
  BURN = 1,
  FREEZE = 2,
  PARALYSIS = 3,
  POISON = 4,
  BAD_POISON = 5,
  SLEEP = 6,
  FAINTING = 7,
};

/** @class 
 * @brief 技を管理するクラス
 * @detail
 *
 */
class Move{
  public:
    //---- getter ----
    bool move_is_physical(){ return move_is_physical_; }
    std::vector<int> up_status(){ return up_status_; }
    int power(){ return power_; }
    int power_poimnt(){ return power_point_; }
    int accuracy(){ return accuracy_; }
    int type(){ return type_; }
    int addition(){ return addition_; }
    int additional_effect_rate() { return additional_effect_rate_; }

  private:
    std::string move_name_;
    std::vector<int> up_status_;
    bool move_is_physical_;     //trueで物理技 falseで特殊技
    int power_;                  //威力
    int accuracy_;               //命中率
    int power_point_;            //技の使用可能回数(PP)
    int type_;                   //技のタイプ
    int addition_;               //能力変化 0:なし 1:能力上昇　2:怯み
    int additional_effect_rate_;   //追加効果の発生確率
};

/** @class
 * @breif ポケモン管理クラス
 * @detail
 * 
 */
class Pokemon{
  public:
    void Initialize(std::string name/*, std::vector<int> EV_input, std::vector<std::string> move_list*/) {
      Name_ = name;
      InitParameter_.resize(6,0);
      CurrentParameter_.resize(6,0);
      EV_.resize(6, 0);
      // EV_ = EV_input;              //努力値のセット
      PU_.resize(6, 1);               //能力上昇の初期化
      IV_.resize(6, 1);               //個体値のデフォルト化
      State_ = NORMAL;
      atk_score_ = 0;
      def_score_ = 0;

      //初期パラメータセット
      for(int itr = 0; itr < 5; itr++){
        if(itr==0){ 
          InitParameter_[itr] =(BS_[itr]*2+EV_[itr]/4+IV_[itr])*(level_/100)+level_+10;
        }
        else {
          InitParameter_[itr] =(BS_[itr]*2+EV_[itr]/4+IV_[itr])*(level_/100)+5;
        }
        CurrentParameter_[itr] = InitParameter_[itr];
      }
    }

    //---- getter -----
    std::string Name() { return Name_; }
    std::vector<int> Currentparameter(){ return CurrentParameter_; }
    std::vector<double> PU(){ return PU_; }
    int level(){ return level_; }
    int type1(){ return type1_; }
    int type2(){ return type2_; }
    StatusAilment status(){ return State_; }
    std::vector<Move> move(){ return move_; }
    double atk_score() { return atk_score_; }
    double def_score() { return def_score_; }

    //---- setter ----
    void set_name(std::string name){ Name_ = name; }
    void set_level(int level){ level_ = level; }
    void set_type1(int type1){ type1_ = type1; }
    void set_type2(int type2){ type2_ = type2; }
    void set_BS(std::vector<int> bs){ BS_ = bs; }
    void set_atk_score(double score){ atk_score_ = score; }
    void set_def_scpre(double score){ def_score_ = score; }

    //相性判定用
    void add_atk_score(double score){ atk_score_ += score; }
    void add_def_score(double score){ def_score_ += score; }

  private:
    constexpr static const int default_ev = 0;       //初期努力値
    constexpr static const int default_iv = 31;      //デフォルト個体値
    constexpr static const int default_bs = 0;       //デフォルト種族値
    constexpr static const double default_pu = 1.0;  //デフォルト能力上昇
    constexpr static const int default_level = 50;   //デフォルトレベル
    std::string Name_;
    std::vector<int> CurrentParameter_;    //現在のパラメータ
    std::vector<int> InitParameter_;       //初期パラメータ
    std::vector<int> EV_;                  //努力値(Efort value)
    std::vector<int> IV_;                  //個体値(Individual value)
    std::vector<int> BS_;                  //種族値(Base status)
    std::vector<double> PU_;               //能力上昇(Power up) 命中 回避を含む
    std::vector<double> Personality_;      //性格補正 A,B,C,D,S,命中,回避
    int happiness_;                        //なつきど
    int level_;                            //レベル defalt: lv.50
    int type1_;                            //タイプ１(0~17の数字の相性テーブルに対応する数字で管理する)
    int type2_;                            //タイプ２
    StatusAilment State_;                  //状態異常
    std::vector<Move> move_;               //わざ

    double atk_score_;                     //相手パーティ全体への攻め相性
    double def_score_;                     //相手パーティ全体への守り相性
};

