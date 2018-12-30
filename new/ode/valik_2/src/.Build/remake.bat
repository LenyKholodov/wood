@echo off
nmake /f make\libmake.vc CFG=Debug   PRECISION=SINGLE
nmake /f make\libmake.vc CFG=Release PRECISION=SINGLE
nmake /f make\libmake.vc CFG=Debug   PRECISION=DOUBLE
nmake /f make\libmake.vc CFG=Release PRECISION=DOUBLE