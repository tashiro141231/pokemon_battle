#include "trainer.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <string>


/** @fn
 * @brief スコア確認 (デバッグ用)
 * @param 
 * @return なし
 * @detail
 *
 */
void Trainer::score(){
  int index = 0;
  std::cout << "_____Interface_____" << std::endl;
  for(auto itr = this->list_.begin(); itr != this->list_.end(); itr++){
    std::cout << this->list_[index].Name() << "'s atk score: " << this->list_[index].atk_score() << std::endl;
    std::cout << this->list_[index].Name() << "'s def score: " << this->list_[index].def_score() << std::endl;
    index++;
  }
}

/** @fn
 * @brief 相手パーティとの相性をチェックする． 
 * @param std::string party_filename: パーティリストが格納されたファイル名
 * @return なし
 * @detail
 */
void Trainer::comp_checker(std::vector<std::string> ene_names){
  int index = 0;
  int user_index = 0;
  int npc_index = 0;

  // 敵パーティのデータを取得
  for(auto itr = ene_names.begin(); itr != ene_names.end(); itr++) {
    this->ene_list_[index] = Trainer::search_pokemon(ene_names[index]);
    if(this->ene_list_[index].Name() == "Error"){
      std::cout << "Couldn't set the ene_list." << std::endl;
    }
    index++;
  }
  // 敵のポケモンとのタイプ相性に関するスコアを算出する
  for(auto user = list_.begin(); user != list_.end(); user++) {
    npc_index = 0;
    for(auto npc = ene_list_.begin(); npc != ene_list_.end(); npc++) {
      // 攻撃スコア
      list_[user_index].add_atk_score(table_check(list_[user_index].type1(), ene_list_[npc_index].type1()));
      list_[user_index].add_atk_score(table_check(list_[user_index].type1(), ene_list_[npc_index].type2()));
      list_[user_index].add_atk_score(table_check(list_[user_index].type2(), ene_list_[npc_index].type1()));
      list_[user_index].add_atk_score(table_check(list_[user_index].type2(), ene_list_[npc_index].type2()));
      // 防御スコア
      list_[user_index].add_def_score(table_check(ene_list_[npc_index].type1(), list_[user_index].type1()));
      list_[user_index].add_def_score(table_check(ene_list_[npc_index].type1(), list_[user_index].type2()));
      list_[user_index].add_def_score(table_check(ene_list_[npc_index].type2(), list_[user_index].type1()));
      list_[user_index].add_def_score(table_check(ene_list_[npc_index].type2(), list_[user_index].type2()));

      npc_index++;
    }
    user_index++;
  }
}

/** @fn
 * @brief 相性テーブルの参照用 
 * @param int atk_type 攻撃側のタイプ int def_type 防御側のタイプ
 * @return 相性関係0~2の倍率
 * @detail
 */
double Trainer::table_check(int atk_type, int def_type){
  if(atk_type == -1 || def_type == -1) {
    return 0;
  }
  return Compatibility_table[atk_type][def_type];
}

/** @fn
 * @brief パーティリストのポケモンを検索してデータをセットする 
 * @param std::string party_filename: パーティリストが格納されたファイル名
 * @return なし
 * @detail
 */
void Trainer::Initialize(std::string party_filename) {
  int index = 0;
  std::string poke_buff;
  std::ifstream party_input(party_filename);

  std::cout << "New trainer registering." << std::endl;

  if(party_input.fail()){
    std::cout << "Could not open " << party_filename << std::endl;
    std::exit(0);
  }

  //ポケモン6匹分の領域を確保
  this->list_.resize(default_pokeall_num);
  this->ene_list_.resize(default_pokeall_num);
  while(getline(party_input, poke_buff)){
    //party_list.csvの最初のデータがポケモン名
    this->list_[index] = Trainer::search_pokemon(Trainer::get_param(poke_buff,0));
    if(this->list_[index].Name() == "Error"){
      std::cout << poke_buff <<" does not exist in the database." << std::endl;
    }
    index++;
  }
  //相性テーブルのセット
  this->table_setter();
  
  std::cout << std::endl;
}

/** @fn
 * @brief データベースからポケモンの検索と必要データをセットしたポケモンオブジェクトを返す
 * @param std::string poke_name ポケモン名(日本語)
 * @return Pokemon poke  Error:poke.Name() = "Error"
 * @detail
 *  データはcsv形式で保存されている．
 */
Pokemon Trainer::search_pokemon(std::string poke_name) {
  Pokemon poke;
  std::string line;
  const std::string filename = "pokemon_database.csv";
  std::ifstream database_poke(filename.c_str());
  const int NUM_COL = 0;
  const int NAME_COL = 1;
  const int TYPE_COL1 = 2;
  const int TYPE_COL2 = 3;
  const int BS_COL = 4;

  while(std::getline(database_poke, line)){
    //マッチ時に名前・タイプ・種族値・レベルをセット
    if(poke_name == get_param(line, NAME_COL)){
      std::cout << "Found Pokemon: " << poke_name << "." << std::endl; 
      poke.set_level(50);     //一旦５０レベルにしとく
      poke.set_type1(Trainer::type_number(get_param(line, TYPE_COL1)));   //タイプ
      poke.set_type2(Trainer::type_number(get_param(line, TYPE_COL2)));   //タイプ
      
      std::vector<int> param_bs(6,0);
      for(int i = 0; i < 6; i++){
        param_bs[i] = std::stoi(get_param(line, BS_COL + i));    //種族値
      }
      poke.set_BS(param_bs);

      //ポケモンの基礎データが揃ったのでイニシャライズ
      poke.Initialize(poke_name);
      //相手トレーナーに渡すようのポケモン名リスト
      if(this->name_list_.size() < 6){
        this->name_list_.push_back(poke_name);
      }
      return poke;
    }
  }
  std::cout << "Couldn't find such name of Pokemon" << std::endl;
  poke.set_name("Error");
  return poke;
}

/** @fn
 * @brief マッチした行データから必要なパラメータを取得する 
 * @param int param_num:パラメータの列番号 
 * @return 取得データ 
 * @detail
 */
std::string Trainer::get_param(std::string line, int param_num) {
  std::string token;
  int param_col = 0;
  
  std::istringstream stream(line);
  while (std::getline(stream, token, ',')) {
    if (param_col == param_num) {
      return token;
    }
    param_col++;
  }
  throw "Could not found such number of param.";
}

/** @fn
 * @brief 
 * @param std::string poke_name ポケモン名(日本語)
 * @return int type_num:各タイプに対応した番号を返す 
 * @detail
 */
int Trainer::type_number(std::string type) {
  int type_num = -1;
  
  if(type == "ノーマル"){
    type_num = 0;
  }
  else if(type == "ほのお"){
    type_num = 1;
  }
  else if(type == "みず"){
    type_num = 2;
  }
  else if(type == "でんき"){
    type_num = 3;
  }
  else if(type == "くさ"){
    type_num = 4;
  }
  else if(type == "こおり"){
    type_num = 5;
  }
  else if(type == "かくとう"){
    type_num = 6;
  }
  else if(type == "どく"){
    type_num = 7;
  }
  else if(type == "じめん"){
   type_num = 8;
  }
  else if(type == "ひこう"){
    type_num = 9;
  }
  else if(type == "エスパー"){
    type_num = 10;
  }
  else if(type == "むし"){
    type_num = 11;
  }
  else if(type == "いわ"){
    type_num = 12;
  }
  else if(type == "ゴースト"){
    type_num = 13;
  }
  else if(type == "ドラゴン"){
    type_num = 14;
  }
  else if(type == "あく"){
    type_num = 15;
  }
  else if(type == "はがね"){
    type_num = 16;
  }
  else if(type == "フェアリー"){
    type_num = 17;
  }
  else if(type == "なし"){
    type_num = -1;
  }

  return type_num;
}

void Trainer::table_setter(){
  std::vector<std::vector<double>> type_table{
  //ノーマル 0
  {1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,0.5,1},
  //ほのお 1
  {1,0.5,0.5,1,2,2,1,1,1,1,1,2,0.5,1,0.5,1,2,1},
  //みず 2
  {1,2,0.5,1,0.5,1,0.5,1,1,1,2,1,1,1,2,1,0.5,1,1,1},
  //でんき 3
  {1,1,2,0.5,0.5,1,1,1,0,2,1,1,1,1,0.5,1,1,1},     
  //くさ 4
  {1,0.5,2,1,0.5,1,1,0.5,2,0.5,1,0.5,2,1,0.5,1,0.5,1},
  //こおり 5
  {1,0.5,0.5,1,2,0.5,1,1,2,2,1,1,1,1,2,1,0.5,1},
  //かくとう 6
  {2,1,1,1,1,2,1,0.5,1,0.5,0.5,0.5,2,0,1,2,2,0.5},
  //どく 7
  {1,1,1,1,2,1,1,0.5,0.5,1,1,1,0.5,0.5,1,1,0,2}, 
  //じめん 8
  {1,2,1,2,0.5,1,1,2,1,0,1,0.5,2,1,1,1,2,1},
  //ひこう 9
  {1,1,1,0.5,2,1,2,1,1,1,1,2,0.5,1,1,1,0.5,1},
  //エスパー 10
  {1,1,1,1,1,1,2,2,1,1,0.5,1,1,1,1,0,0.5,1},  
  //むし 11
  {1,0.5,1,1,2,1,0.5,0.5,1,0.5,2,1,1,0.5,1,2,0.5,0.5}, 
  //いわ 12
  {1,2,1,1,1,2,0.5,1,0.5,2,1,2,1,1,1,1,0.5,1},
  //ゴースト 13
  {0.5,1,1,1,1,1,1,1,1,1,2,1,1,2,1,0.5,1,1},
  //ドラゴン 14
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,1,0.5,0},  
  //あく 15
  {1,1,1,1,1,1,0.5,1,1,1,2,1,1,2,1,0.5,1,0.5},
  //はがね 16
  {1,0.5,0.5,0.5,1,2,1,1,1,1,1,1,2,1,1,1,0.5,2},
  //フェアリー 17
  {1,0.5,1,1,1,1,2,0.5,1,1,1,1,1,1,2,2,0.5,1}
  };
  Compatibility_table = type_table;
}
