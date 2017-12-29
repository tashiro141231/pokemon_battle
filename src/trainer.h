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
#include <fstream>
#include <cstdlib>
#include "pokemon.h"

class Trainer{
  public:
    //---- ポケモンのデータベースからの検索とデータのセット用
    void Initialize(std::string party_filename);
    Pokemon search_pokemon(std::string poke_name);
    std::string get_param(std::string line, int param_num);
    int type_number(std::string type);
    void table_setter();
    
    //---- getter ----
    int num_pokemon() { return num_pokemon_; }
    std::vector<std::string> name_list() { return name_list_; }
    std::string list_pokename(int index) { return list_[index].Name(); }
    
    //---- setter ----
    //選択したポケモンへのインデックスを保存
    void set_selected_poke(int *selected) {
      for(int i=0; i< 3;i++){
        poke_list_[i] = selected[i];
      }
    }

    //相性チェック用関数
    void comp_checker(std::vector<std::string> ene_names);
    double table_check(int atk_type, int def_type);

    //Interface
    void score();

  private:
    std::vector<Pokemon> list_;
    std::vector<Pokemon> ene_list_;
    std::vector<std::string> name_list_;
    int num_pokemon_;      //残りのポケモン数
    int poke_list_[3];     //選択した３体のポケモンへのインデックス
    constexpr static int default_pokeall_num = 6;
    std::vector<std::vector<double>> Compatibility_table;
};
