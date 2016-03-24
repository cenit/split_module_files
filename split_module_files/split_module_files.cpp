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

void print_module_files(std::vector<std::string>&, std::vector<std::vector<std::string>>&);
void check_duplicate_modules(std::vector<std::string>&);
void split_modules(std::string&, std::vector<std::string>&, std::vector<std::vector<std::string>>&);
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

  std::vector<std::string> module_names;
  std::vector<std::vector<std::string>> parsed;
  for (auto& filename : filenames) split_modules(filename, module_names, parsed);
  check_duplicate_modules(module_names);
  print_module_files(module_names, parsed);
  return 0;
}



void print_module_files(std::vector<std::string>& module_names, std::vector<std::vector<std::string>>& parsed) {
  boost::filesystem::path folder = SPLITTED_FOLDERNAME;

  if (!boost::filesystem::exists(folder)) {
    boost::filesystem::create_directories(folder);
  }

  for (size_t i = 0; i < parsed.size(); i++) {
    std::stringstream modulename;
#ifdef _WIN32
    modulename << std::string(SPLITTED_FOLDERNAME) << std::string("\\") << module_names[i];
#else
    modulename << std::string(SPLITTED_FOLDERNAME) << std::string("/") << module_names[i];
#endif
    if (enable_debug) std::cout << "Opening file " << modulename.str() << " to write " << parsed[i].size() << " lines" << std::endl;
    std::ofstream modulo(modulename.str());
    for (auto riga : parsed[i]) modulo << riga << std::endl;
    modulo.close();
  }
}


void check_duplicate_modules(std::vector<std::string>& module_names) {
  for (size_t i = 0; i < module_names.size(); i++) {
    for (size_t j = i+1; j < module_names.size(); j++) {

      std::string extension_1 = module_names[i].substr(module_names[i].size() - 4, 4);
      std::string extension_2 = module_names[j].substr(module_names[j].size() - 4, 4);
      std::string module_name_1 = module_names[i].substr(0, module_names[i].size() - 4);
      std::string module_name_2 = module_names[j].substr(0, module_names[j].size() - 4);

      if (module_name_1 == module_name_2) module_name_2 += "_2";  // the check is done only on the filename (no extensions) to avoid name clashes on windows between a.f90 and a.F90
      module_names[j] = module_name_2 + extension_2;
    }
  }
}

void split_modules(std::string& file_name, std::vector<std::string>& module_names, std::vector<std::vector<std::string>>& parsed) {
  std::ifstream file_to_parse(file_name, std::ios::in);
  if (!file_to_parse) {
    std::cout << "Cannot open " << file_name << ". Quitting..." << std::endl;
    exit(-2);
  }

  std::string extension = file_name.substr(file_name.size() - 4, 4);

  std::string line, original_line;
  std::vector<std::string> tokens;
  std::vector<std::string> modulo;

  while (getline(file_to_parse, line)) {
    original_line = line;
    boost::algorithm::trim(line);
    boost::algorithm::split(tokens, line, boost::algorithm::is_any_of(" "), boost::token_compress_on);
    if (( tokens[0] == "module" || tokens[0] == "program" ) && tokens.size() > 1) {
      module_names.push_back(tokens[1]+extension);
    }
    modulo.push_back(original_line);
    if ((tokens[0] == "end" && tokens.size() > 1 && tokens[1] == "module") || (tokens[0] == "end" && tokens.size() > 1 && tokens[1] == "program")) {
      parsed.push_back(modulo);
    }
    line.clear(); tokens.clear();
  }
  file_to_parse.close();
  if (enable_debug) std::cout << "Parsed file " << file_name << ", produced " << parsed.size() << " module files." << std::endl;

}


