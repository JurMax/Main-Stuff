/*
 * JPlatform.hpp
 *
 *  Created on: 25 Apr 2017
 *      Author: Jurriaan en Maxim
 */

#ifndef OVERTURE_INCLUDE_JPLATFORM_HPP_
#define OVERTURE_INCLUDE_JPLATFORM_HPP_

#include <string>

void Platform_Init();

//TODO: Maak dit een enum
std::string Platform_GetOS();

void Platform_LoadDragAndDrop();
void Platform_Drag( std::string file );


#endif /* OVERTURE_INCLUDE_JPLATFORM_HPP_ */
