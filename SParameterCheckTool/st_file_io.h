/*
 *Author:spc
 *Description: Read and write S parameter file.
 */

#pragma once

#include <QString>


void readSparameter(QString filePath,struct snp& data);

void writeSparameter(QString filePath,struct snp& data);

