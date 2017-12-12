/** 
 * @file trainer.h
 * @class トレーナーを管理するクラス
 * @author Yosuke Tashiro
 * @date 2017/12/11
 * @detail
 */

#include <ros/ros.h>
#include <string>
#include <vector>
#include "pokemon.h"

class Trainer{
  public:
    Trainer(){
      std::vector<Pokemon> list;
    }
    //---- getter ----
    int num_pokemon_(){ return num_pokemon; }
    
    //---- setter ----
    //選択したポケモンへのインデックスを保存
    void set_selected_poke(int *selected){
      for(int i=0; i< 3;i++){
        poke_list[i] = selected[i];
      }
    }

    //対戦用
    void attack_(Trainer ene);

  private:
    int num_pokemon;      //残りのポケモン数
    static int poke_list[3];     //選択した３体のポケモンへのインデックス
    static std::vector<Trainer> ene_list;
};
