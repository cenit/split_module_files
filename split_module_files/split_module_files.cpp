/*                 Copyright 2016 - Stefano Sinigardi                  */

/************************************************************************
* This program is free software: you can redistribute it and/or modify  *
* it under the terms of the GNU General Public License as published by  *
* the Free Software Foundation, either version 3 of the License, or     *
* (at your option) any later version.                                   *
*                                                                       *
* This program is distributed in the hope that it will be useful,       *
* but WITHOUT ANY WARRANTY; without even the implied warranty of        *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
* GNU General Public License for more details.                          *
*                                                                       *
* You should have received a copy of the GNU General Public License     *
* along with this program.  If not, see <http://www.gnu.org/licenses/>. *
************************************************************************/

#define _SCL_SECURE_NO_WARNINGS
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <boost/algorithm/string.hpp> 
#include <boost/filesystem.hpp>

#define SPLITTED_FOLDERNAME "split"

void split_modules(std::string &);
bool enable_debug = true;

int main(int argc, char*argv[]) {

  if (argc < 2) {
    std::cout << "Usage: " << argv[0] << " *.f90\nThe tool will create a folder here, named \"split\", which will contain"
      << " one file for every module defined in the input files" << std::endl;
    exit(-2);
  }

  std::vector<std::string> filenames;
  for (int i = 1; i < argc; i++) {
    filenames.push_back(argv[i]);
  }

  for (auto& filename : filenames) split_modules(filename);
  return 0;
}




void split_modules(std::string & file_name) {
  std::ifstream file_to_parse(file_name, std::ios::in);
  if (!file_to_parse) {
    std::cout << "Cannot open " << file_name << ". Quitting..." << std::endl;
    exit(-2);
  }

  std::string extension = file_name.substr(file_name.size() - 4, 4);

  std::string line, original_line;
  std::vector<std::string> tokens;
  std::vector<std::vector<std::string>> parsed;
  std::vector<std::string> module_names;
  std::vector<std::string> modulo;

  while (getline(file_to_parse, line)) {
    original_line = line;
    boost::algorithm::trim(line);
    boost::algorithm::split(tokens, line, boost::algorithm::is_any_of(" "), boost::token_compress_on);
    if (tokens[0] == "module" && tokens.size() > 1) {
      module_names.push_back(tokens[1]);
    }
    modulo.push_back(original_line);
    if (tokens[0] == "end" && tokens.size() > 1 && tokens[1] == "module") {
      parsed.push_back(modulo);
    }
    line.clear(); tokens.clear();
  }
  file_to_parse.close();
  if (enable_debug) std::cout << "Parsed file " << file_name << ", produced " << parsed.size() << " module files." << std::endl;

  boost::filesystem::path folder = SPLITTED_FOLDERNAME;

  if (!boost::filesystem::exists(folder)) {
    boost::filesystem::create_directories(folder);
  }

  for (size_t i = 0; i < parsed.size(); i++) {
    std::stringstream modulename;
#ifdef _WIN32
    modulename << std::string(SPLITTED_FOLDERNAME) << std::string("\\") << module_names[i] << extension;
#else
    modulename << std::string(SPLITTED_FOLDERNAME) << std::string("/") << module_names[i] << extension;
#endif
    if (enable_debug) std::cout << "Opening file " << modulename.str() << " to write " << parsed[i].size() << " lines" << std::endl;
    std::ofstream modulo(modulename.str());
    for (auto riga : parsed[i]) modulo << riga << std::endl;
    modulo.close();
  }
}


