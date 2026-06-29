#pragma once
#include "ECS/Types.h"

/** 
* @brief ECS ::Conponents/Camera.h
* Camera- componente de DATOS PUROS. Convierte a una entidad 
* en una cámara. Su POSICIÓN vive en el Transform de la misma entidad 
* (el centro de la vista). Aquí solo van los ajustes
* propios de la cámara.
* 
* El ÇameraSystem busca la cámara activa, interpola su Tranform hacia
* el objetivo (si lo hay) y aplica la visa 
* a la ventana 
*/

namespace ECS {
	struct Camera {
		//zoom
		float zoom{ 1.f }; 

		//solo una camata debería estar activa a la vez
		bool active{ true }; 

		//Entidad a seguir. NULL_ENTITY = cámara libre (no sigue a nadie)
		EntityID followTarget{ NULL_ENTITY }; 

		//Velocidad de seguimiento. Mayor = se pega más rápido al objetivo
		//Es framerate-independet (ver CameraSYSTEM), NO ES PX/SEG. 
		float followSpeed{ 5.f }; 
	};
}