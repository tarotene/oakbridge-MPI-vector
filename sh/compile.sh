#!/bin/bash

mpiicc ./src/global-two-norm.c -o ./bin/global-two-norm.out
mpiicc ./src/global-vector.c -o ./bin/global-vector.out