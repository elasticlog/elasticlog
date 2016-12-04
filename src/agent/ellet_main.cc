/*
 * ellet_main.cc
 * Copyright (C) 2016 elasticlog <elasticlog01@gmail.com>
 *
 * Distributed under terms of the GNU GENERAL PUBLIC LICENSE.
 */

#include "ellet_impl.h"


int main(int argc, char* args[]) {
  ::el::ElLetImpl* ellet = new ::el::ElLetImpl();
  ellet->Init();
  printf("hello ellet!\n");
  return 0;
}



/* vim: set expandtab ts=2 sw=2 sts=2 tw=100: */
