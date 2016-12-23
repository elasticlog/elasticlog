/*
 * file_utils.h
 * Copyright (C) 2016 elasticlog <elasticlog01@gmail.com>
 *
 * Distributed under terms of the GNU GENERAL PUBLIC LICENSE.
 */

#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <string>

namespace el {

bool Mkdir(const std::string& path);
bool MkdirRecur(const std::string& path);

}

#endif /* !FILE_UTILS_H */

/* vim: set expandtab ts=2 sw=2 sts=2 tw=100: */
