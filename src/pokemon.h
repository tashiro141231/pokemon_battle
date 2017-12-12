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
    Move(){
      ;
    }
    //---- getter ----
    bool move_is_physical_(){ return move_is_physical; }
    std::vector<int> up_status_(){ return up_status; }
    int power_(){ return power; }
    int power_poimnt_(){ return power_point; }
    int accuracy_(){ return accuracy; }
    int type_(){ return type; }
    int addition_(){ return addition; }
    int addtion_effect_rate_() { return addition_effect_rate; }

  private:
    static std::string move_name;
    static std::vector<int> up_status;
    bool move_is_physical;     //trueで物理技 falseで特殊技
    int power;                  //威力
    int accuracy;               //命中率
    int power_point;            //技の使用可能回数(PP)
    int type;                   //技のタイプ
    int addition;               //能力変化 0:なし 1:能力上昇　2:怯み
    int addition_effect_rate;   //追加効果の発生確率
};

/** @class
 * @breif ポケモン管理クラス
 * @detail
 * 
 */
class Pokemon{
  public:
    Pokemon(std::string name, std::vector<int> EV_input, std::vector<std::string> move_list) {
      //
      //名前と技のリストからPU,moveを取得する処理
      //
      
      EV = EV_input; 

      //初期パラメータセット
      for(int itr = 0; itr < 5; itr++){
        if(itr==0){ 
          InitParameter[itr] =(BS[itr]*2+EV[itr]/4+IV[itr])*(level/100)+level+10;
        }
        else {
          InitParameter[itr] =(BS[itr]*2+EV[itr]/4+IV[itr])*(level/100)+5;
        }
        CurrentParameter[itr] = InitParameter[itr];
      }
    }

    //---- getter -----
    std::vector<int> Currentparameter_(){ return CurrentParameter; }
    std::vector<int> PU_(){ return PU; }
    int level_(){ return level; }
    int type1_(){ return type1; }
    int type2_(){ return type2; }
    StatusAilment status_(){ return State; }
    std::vector<Move> move_(){ return move; }

  private:
    static std::vector<int> CurrentParameter;    //現在のパラメータ
    static std::vector<int> InitParameter;       //初期パラメータ
    static std::vector<int> EV;                  //努力値(Efort value)
    static std::vector<int> IV;                  //個体値(Individual value)
    static std::vector<int> BS;                  //種族値(Base status)
    static std::vector<int> PU;                  //能力上昇(Power up) 命中 回避を含む
    int happiness;                        //なつきど
    int level;                            //レベル defalt: lv.50
    int type1;                            //タイプ１(0~17の数字の相性テーブルに対応する数字で管理する)
    int type2;                            //タイプ２
    StatusAilment State;                  //状態異常
    std::vector<Move> move;               //わざ
};

