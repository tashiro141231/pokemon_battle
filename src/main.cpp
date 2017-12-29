#include <stdio.h>
#include <iostream>
#include <fstream>
#include <curl/curl.h>
#include <iconv.h>
#include <string.h>
#include <ros/ros.h>

#include "trainer.h"

#define FRAME_RATE 10

// #define DATA_COLLECTION_MODE    //コメントアウトを解除してデータ収集モード(オンラインで実行してください)

//データ収集用の関数の宣言
char mega_char_getter(std::vector<std::string> mega_list, std::string pre_csv_line);
char except_char_getter(std::string pre_csv_line);
bool get_html(int zukan_num, std::string zukan_filename, char mega_char, char except_char);
bool conv_to_UTF8(int zukan_num, std::string zukan_filename, std::string utf8_filename);
std::string parse_html(std::string utf8_filename);
std::string name_getter(std::string line_buff);
std::string type_getter(std::string line_buff);
std::string bs_getter(std::string line_buffer, std::string name_param);

#ifndef DATA_COLLECTION_MODE
// 通常
int main(int argc, char **argv) {
  Trainer user;
  Trainer npc;
  std::string your_party = "party_list.csv";
  std::string npc_party = "party_list1.csv";

  user.Initialize(your_party);
  npc.Initialize(npc_party);

  user.comp_checker(npc.name_list());
  npc.comp_checker(user.name_list());

  user.score();
  std::cout << std::endl;
  npc.score();

  while(1){
    ;
  }
}

#else
//技、ポケモンリストのインターネット経由で取得し保存
//データ元：https://yakkun.com/sm/zukan/
int main(int argc, char **argv) {
  //htmlデータ取得用変数
  const int end_of_zukan_number = 806;
  int zukan_index = 1;    //図鑑はNo.1から802まで現状存在
  
  //ファイル出力用変数
  std::string database_filename = "pokemon_database.csv";
  std::string zukan_filename = "zukan.html";  //curlでとってきたソースを保存するファイル名
  std::string utf8_filename = "zukan.txt";    //UTF-8に変換したものを一時保存するファイル名
  std::ofstream csv_file(database_filename);
 
  char except_char = 'n';
  
  //メガシンカ検出用
  int m_index = 0;
  char mega_char = 'n';
  std::string mega_filename = "mega_list.txt";
  std::string mega_buff;
  std::string pre_csv_line;
  std::vector<std::string> mega_list;
  std::ifstream mega_file(mega_filename);

  //メガポケモンのリストをファイルから取得
  while(mega_file && getline(mega_file, mega_buff)) {
    mega_list.push_back(mega_buff);
    m_index++;
  }

  //全ポケモンのデータをネット経由で取得
  while(zukan_index<=end_of_zukan_number){
    std::string csv_line;

    //一回目のループはpre_csv_lineがないので例外処理
    if(zukan_index != 1){
      mega_char = mega_char_getter(mega_list, pre_csv_line);
      except_char = except_char_getter(pre_csv_line);
      //メガシンカがないとき、もうメガシンカのデータをとったとき
      if(mega_char != 'n' || except_char != 'n'){
        zukan_index--;
      }
    }

    //htmlの取得
    if(!get_html(zukan_index, zukan_filename, mega_char, except_char)){
      std::cout << "Couldn't get data at No." << zukan_index  << std::endl;
      break;
    }
    else{
      //取得したhtmlファイルをutf8形式の変換
      if(conv_to_UTF8(zukan_index, zukan_filename, utf8_filename)){
        csv_line = parse_html(utf8_filename);  //名前・タイプ・種族値を抜き出す
        csv_line = std::to_string(zukan_index) + "," + csv_line;
        pre_csv_line = csv_line;
        csv_file << csv_line << std::endl;     //データを保存
      }
      else{
        std::cout << "Could not convert to UTF8." << std::endl;
        break;
      }
    }
    zukan_index++;
  }
  return 0;
}
#endif

/** @fn
 * @brief メガシンカ時の添字を返す
 * @param std::vector<std::string>メガシンカポケモンリスト std::string pre_csv_line一つ前に取得したデータ
 * @return char mege_char:'m':通常メガシンカ 'x':メガ〜X 'y':メガ〜Y 'n':メガシンカはない
 * @detail
 */
char mega_char_getter(std::vector<std::string> mega_list, std::string pre_csv_line){
  char mega_char = 'n';
  int index = 0;

  //メガシンカがあるか検索
  for(auto itr = mega_list.begin(); itr != mega_list.end(); ++itr) {
    if(pre_csv_line.find(mega_list[index]) != std::string::npos ){
      if(pre_csv_line.find('X') != std::string::npos){
        mega_char = 'y';
      }
      else if(pre_csv_line.find('Y') != std::string::npos){
        mega_char = 'n';
      }
      else if(pre_csv_line.find("リザードン") != std::string::npos || pre_csv_line.find("ミュウツー") != std::string::npos){
        mega_char = 'x';
      }
      else if(pre_csv_line.find("メガ") != std::string::npos){
        mega_char = 'n';
      }
      else{
        mega_char = 'm';
      }
    }
    index++;
  }
 
  return mega_char;
}

/** @fn
 * @brief アローラの姿などがあるときの添字を返す
 * @param std::string pre_csv_line一つ前に取得したデータ
 * @return char except_char:'a':アローラの姿があるとき 'n':アローラの姿はない
 * @detail
 */
char except_char_getter(std::string pre_csv_line) {
  char except_char = 'n';

  //別のフォルムを持つポケモン達の処理
  if(pre_csv_line.find("通常") != std::string::npos) {
    except_char= 'a';
  }
  if(pre_csv_line.find("けしん") != std::string::npos){
    except_char = 'a';
  }
  if(pre_csv_line.find("ブラックキュレム") != std::string::npos){
    except_char = 'w';
  }
  else if(pre_csv_line.find("ホワイトキュレム") != std::string::npos){
    except_char = 'n';
  }
  else if(pre_csv_line.find("キュレム") != std::string::npos) {
    except_char = 'b';
  }
  if(pre_csv_line.find("ゲッコウガ") != std::string::npos) {
     except_char = 'n';
  }
  if(pre_csv_line.find("ネクロズマ") != std::string::npos) {
    except_char = 'n';
  }
  
  return except_char;
}

/** @fn
 * @brief 指定の番号のポケモンのデータをhtmlファイルとして保存 
 * @param zukan_num:指定の図鑑番号 zukan_filename:取得したページ保存するファイル名
 * @return true:成功 false:失敗
 * @detail
 */
bool get_html(int zukan_num, std::string zukan_filename, char mega_char, char except_char){
  //curl用変数
  CURL *curl;
  CURLcode res;
  long http_code = 0;
  
  //urlの指定とその保存用
  FILE *fp;
  char poke_url[256] ="https://yakkun.com/sm/zukan/n";
  char poke_num[5];

  fp = fopen(zukan_filename.c_str(), "w"); 


  //指定番号へのURLの生成とcurlの設定
  sprintf(poke_num, "%d", zukan_num);
  strcat(poke_url, poke_num);
  //メガシンカの文字 'n'でメガシンカではない
  if(mega_char != 'n'){
    strcat(poke_url, &mega_char);
  }
  if(except_char != 'n'){
    strcat(poke_url, &except_char);
  }

  curl = curl_easy_init();
  if ( curl ==NULL ){
    std::cout << "Failed to init curl" << std::endl;
    return false;
  }
  curl_easy_setopt(curl, CURLOPT_URL, poke_url);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, fwrite);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
  res = curl_easy_perform(curl);    //実行
  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);  //ステータスコードの取得
  curl_easy_cleanup(curl);    //ハンドラのクリーンアップ
  fclose(fp);
  std::cout << poke_url << std::endl;
  // std::cout << zukan_filename << " has been created." << std::endl;

  return true;
}

/** @fn
 * @brief 取得したhtmlファイルをEUC-JP形式からUTF-8形式に変換する
 * @param zukan_num:指定の図鑑番号 zukan_filename:入力ファイル名
 *        output_file:出力ファイル名
 * @return true:成功 false:失敗
 * @detail curlのサンプルがC言語のサンプルだったので一旦C言語で実装しとく
 */
bool conv_to_UTF8(int zukan_num, std::string zukan_filename, std::string utf8_filename){
  const int max_char_in_line = 1024;    //fgets()するときの行の最大文字数
  iconv_t icd;
  FILE *fp_input, *fp_output;
  char *ptr_input, *ptr_output;
  size_t length_input, length_output;
  char string_input[max_char_in_line], string_output[max_char_in_line];

  //変換前ファイルを開く
  if((fp_input = fopen(zukan_filename.c_str(), "r")) == NULL){
    std::cout << "Could not create" << zukan_filename << "." << std::endl;
    return false;
}
  
  //変換後のデータを保存先を開いておく
  if((fp_output = fopen(utf8_filename.c_str(), "w")) == NULL){
    std::cout << "Could not create "<< utf8_filename << "." << std::endl;
    return false;
  }
  
  //EUC-JPからUTF-8にエンコードする宣言
  icd = iconv_open("UTF-8","EUC-JP");
  
  //ファイルが終わるまでループ
  while(1){
    fgets(string_input, max_char_in_line, fp_input);   //1ラインずつエンコード
    //とれなければファイルの終わり
    if(feof(fp_input)){
      break;
    }
    ptr_input = string_input;
    ptr_output = string_output;
    length_input = strlen(string_input);
    length_output = max_char_in_line - 1;

    iconv(icd, &ptr_input, &length_input, &ptr_output, &length_output);   //エンコード

    *ptr_output = '\0';
    fputs(string_output, fp_output);      //書き込み
  }
  //ファイルクローズ
  fclose(fp_input);
  fclose(fp_output);

  return true;
}

/** @fn
 * @brief htmlファイルから解析して、データベースに必要なデータを取得する
 * @param utf8_filename:utf8形式のソースファイル
 * @return csv_line:必要データをコンマで区切った文字列
 * @detail データをかなりゴリ押しで情報を抜き出していく
 */
std::string parse_html(std::string utf8_filename){
  //解析用
  std::string csv_line;
  std::string line_buff;
  std::string word_buff;
  std::string bs_buff;
  std::ifstream input_file(utf8_filename);

  //エラー処理
  if(input_file.fail()) {
    std::cout << "Could not open " << utf8_filename << "." << std::endl;
  }

  //ファイル内の探索ループ
  int i = 0;
  while(getline(input_file, line_buff)){
    i++;
    //名前の検索 ページタイトルになっているのでそこから抜き出す
    if(line_buff.find("<title>") != std::string::npos ) {
      word_buff = name_getter(line_buff);
      csv_line += word_buff;
      csv_line += ",";
    }
 
    //タイプを抜き出す
    if(line_buff.find(">タイプ<") != std::string::npos){
      word_buff = type_getter(line_buff);
      csv_line += word_buff;
      csv_line += ",";
    }

    //HP種族値を抜き出す
    if(line_buff.find(">HP<") != std::string::npos){
      bs_buff = "HP";
      word_buff = bs_getter(line_buff, bs_buff);
      csv_line += word_buff;
      csv_line += ",";
    }
    //こうげき種族値を抜き出す
    if(line_buff.find(">こうげき<") != std::string::npos){
      bs_buff = "こうげき";
      word_buff = bs_getter(line_buff, bs_buff);
      csv_line += word_buff;
      csv_line += ",";
    }
    //ぼうぎょ種族値を抜き出す
    if(line_buff.find(">ぼうぎょ<") != std::string::npos){
      bs_buff = "ぼうぎょ";
      word_buff = bs_getter(line_buff, bs_buff);
      csv_line += word_buff;
      csv_line += ",";
    }
    //とくこう種族値を抜き出す
    if(line_buff.find(">とくこう<") != std::string::npos){
      bs_buff = "とくこう";
      word_buff = bs_getter(line_buff, bs_buff);
      csv_line += word_buff;
      csv_line += ",";
    }
    //とくぼう種族値を抜き出す
    if(line_buff.find(">とくぼう<") != std::string::npos){
      bs_buff = "とくぼう";
      word_buff = bs_getter(line_buff, bs_buff);
      csv_line += word_buff;
      csv_line += ",";
    }
    //すばやさ種族値を抜き出す
    if(line_buff.find(">すばやさ<") != std::string::npos){
      bs_buff = "すばやさ";
      word_buff = bs_getter(line_buff, bs_buff);
      csv_line += word_buff;
      break;
    }
  }
  // std::cout << "CSV line is " << csv_line << std::endl;
  return csv_line;
}

/** @fn
 * @brief 名前を抜き出す
 * @param line_buff:名前を含む文字列
 * @return poke_name:ポケモン名
 * @detail データをゴリ押しで情報を抜き出していく
 */
std::string name_getter(std::string line_buff){
  int index = 0;
  std::string poke_name;

  //'>'がでてくるまでインデックスをすすめる
  while(line_buff[index] != '>' ){
    index++;
  }
  index++;            //もうひとつインデックスをすすめる
  //'&'がでるまでポケモン名
  while(line_buff[index] != '&') {
    //カプ系のポケモンのときカプ〜（カプ〜）という表記になっている仕様に対処
    if(line_buff[index] == '(' && (line_buff.find("カプ") != std::string::npos)){
      break;
    }
    poke_name += line_buff[index];
    index++;
  }
  // std::cout << line_buff << std::endl;
  // std::cout << poke_name << std::endl;
  return poke_name;
}

/** @fn
 * @brief タイプを抜き出す
 * @param line_buff:名前を含む文字列
 * @return std::string:ポケモンのタイプ
 * @detail タイプが２つある場合は','で区切る
 * html原文 <tr class="center"><td class="c1">タイプ</td><td><ul class="type"><li><a href="/sm/zukan/search/?type=11"><img src="//img.yakkun.com/poke/xy_type/n11.gif" alt="くさ" /></a></li><li><a href="/sm/zukan/search/?type=3"><img src="//img.yakkun.com/poke/xy_type/n3.gif" alt="どく" /></a></li></ul></td></tr>
c
 */
std::string type_getter(std::string line_buff) {
  int index = 0;
  std::string type_buff;

  //altがでるまでインデックスをすすめる
  while(1){
    if(line_buff[index] == 'a' && line_buff[index+1] == 'l' && line_buff[index+2] =='t'){
      break;
    }

    else {
      index++;
    }
  }
  index = index + 5;    //alt="タイプ" />の形から抜き出すのでタイプまでインデックスをすすめる
  //""で囲まれている間はタイプ
  while(line_buff[index] != '"'){
    // std::cout << line_buff[index]<< std::endl;
    type_buff += line_buff[index];
    index++;
  }
  
  //2つめのタイプがあるかチェック・存在したら追加
  while(1){
    if(line_buff[index] == 'a' && line_buff[index+1] == 'l' && line_buff[index+2] == 't'){  //２つ目のタイプを持っているとき
      type_buff += ",";     // ','で区切る
      index = index + 5;    //同上の処理
      while(line_buff[index] != '"'){
        type_buff += line_buff[index];
        index++;
      }
      break;
    }
    //行の終わりに達したら２つめのタイプはない
    if(line_buff[index] == 't' && line_buff[index+1] == 'r' && line_buff[index+2] == '>'){
      type_buff += ",";
      type_buff += "なし";
      break;
    }
    index++;
  }

  //タイプは最大２つなので終了
  return type_buff;
}

/** @fn
 * @brief 種族値を抜き出す
 * @param line_buff:名前を含む文字列
 * @return poke_name:ポケモン名
 * @detail データをゴリ押しで情報を抜き出していく
 */
std::string bs_getter(std::string line_buff, std::string name_param){
  int index = 0;
  std::string param;

  //2回目の';'の直後から種族値はスタートする(ただしHPのラインだけ3つめの";")
  while(line_buff[index] != ';') {
    index++;
  }
  index++;
  while(line_buff[index] != ';') {
    index++;
  }
  index++;
  //HPの種族値のときHTMLの仕様上対策
  if(name_param == "HP"){
    while(line_buff[index] != ';') {
      index++;
    }
    index++;
  }
  //std::cout << "line_buff = " << line_buff << std::endl;
  //'0'(48) ~ '9'(57)がでてる間、種族値
  while(line_buff[index] >= 48 && line_buff[index] <= 57){
    param += line_buff[index];
    index++;
  }
  // std::cout << name_param << " = " << ret << std::endl;
  return param; 
}

