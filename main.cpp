#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <math.h>
#include <tuple>
#include <climits>
#include <time.h>

using namespace std;

//Variables Globales

int B, Q, P, S, E; 
vector<int> busesEstacion; 
int personasTotalPE;
vector<int> personasPE;
int capacidadTotalS;
vector<int> capacidadS;

vector< vector<int> > distEstacionPtoEncuentro;
vector< vector<int> > distPtoEncuentroRefugio;

tuple<int, int, int> lastSWAP (-1, 0, 1); //(bus, elemento1, elemento2)

// Funcion Trim ////////////////////////////////////////////////////////////////////////////////
// Descripcion: elimina los espacios en blanco al inicio y al final de un string
// fuente: https://gist.github.com/dedeexe/9080526
// fecha: 24/06/2017

string trim_left(const string& str)
{
  const string pattern = " \f\n\r\t\v";
  return str.substr(str.find_first_not_of(pattern));
}

string trim_right(const string& str)
{
  const string pattern = " \f\n\r\t\v";
  return str.substr(0,str.find_last_not_of(pattern) + 1);
}


string trim(const string& str)
{
  return trim_left(trim_right(str));
}

//////////////////////////////////////////////////////////////////////////////////////////////////

// Funcion Split ////////////////////////////////////////////////////////////////////////////////
// Descripcion: genera un vector con los componentes resultates del split del texto en base a un caracter
// fuente: http://code.runnable.com/VHb0hWMZp-ws1gAr/splitting-a-string-into-a-vector-for-c%2B%2B
// fecha: 23/06/2017
// Modificiado al integrar Trim

vector<string> split(string str, char delimiter) {

  str = trim(str);
  vector<string> internal;
  stringstream ss(str); // Turn the string into a stream.
  string tok;
  
  while(getline(ss, tok, delimiter)) {
    tok = trim(tok);
    internal.push_back(tok);
  }
  
  return internal;
}
//////////////////////////////////////////////////////////////////////////////////////////////////

vector<int> obtener_lista(string str) {
  str = trim(str);
  vector<int> internal;
  stringstream ss(str); // Turn the string into a stream.
  string tok;
  
  while(getline(ss, tok, ' ')) {
    tok = trim(tok);
    internal.push_back(atoi(tok.c_str()));
  }
  
  return internal;
}

void lectura_instacia(char *archivo){
  
  ifstream instanciaFile(archivo);

  if (instanciaFile.is_open()){
    string line;
    vector<string> data; 

    getline(instanciaFile, line);
    
    data = split(line, ':');

    B = atoi(data[0].c_str());
    Q = atoi(data[1].c_str());


    getline(instanciaFile, line);

    data = split(line, ':');

    E = atoi(data[0].c_str());

    busesEstacion = obtener_lista(data[1]);


    getline(instanciaFile, line);

    data = split(line, ':');

    P = atoi(data[0].c_str());
    personasTotalPE = atoi(data[1].c_str());

    personasPE = obtener_lista(data[2]);

    
    getline(instanciaFile, line);

    data = split(line, ':');

    S = atoi(data[0].c_str());
    capacidadTotalS = atoi(data[1].c_str());

    capacidadS = obtener_lista(data[2]);

    getline(instanciaFile, line);

    for(int i = 0; i < E; ++i){
      getline(instanciaFile, line);
      data = split(line, ':');
      distEstacionPtoEncuentro.push_back(obtener_lista(data[1]));
    }

    getline(instanciaFile, line);

    for(int i = 0; i < P; ++i){
      getline(instanciaFile, line);
      data = split(line, ':');
      distPtoEncuentroRefugio.push_back(obtener_lista(data[1]));
    }
  }

  else cout << "No se pudo abrir el archivo " << archivo << endl;

}

void imprimirVector(vector<int> lista){
	cout << endl;
	for(int i = 0; i < lista.size(); ++i)
		cout << lista[i] << "  ";
	cout << endl;
}

void imprimir_sol(vector< vector< tuple<int, int> > > solInicial){
	for(int i = 0; i < solInicial.size(); ++i){
		cout << i+1 << ": ";
		for(int j = 0; j < solInicial[i].size(); ++j){
			cout << "(" << get<0>(solInicial[i][j])+1 << "," << get<1>(solInicial[i][j])+1 << "), ";  
		}
		cout << endl;
	}
}

bool es_factible(vector< vector< tuple<int, int> > > candidatoSolucion){
	
	vector<int> personasPorSalvar = personasPE;
	vector<int> capacidadRestante = capacidadS;

	int genteBus;

	for(int i = 0; i < candidatoSolucion.size(); ++i){
		for(int j = 0; j < candidatoSolucion[i].size(); ++j){
			
			if(personasPorSalvar[get<0>(candidatoSolucion[i][j])] == 0){ //No sebe ir a un pto de encuento vacio 
				cout << "el punto de encuento " << get<0>(candidatoSolucion[i][j])+1 << " esta vacio " << endl;
				return false;
			}
			else if(personasPorSalvar[get<0>(candidatoSolucion[i][j])] < Q){
				genteBus = personasPorSalvar[get<0>(candidatoSolucion[i][j])];
			} 
			else{
				genteBus = Q;
			}

			personasPorSalvar[get<0>(candidatoSolucion[i][j])]-= genteBus;
			capacidadRestante[get<1>(candidatoSolucion[i][j])]-= genteBus;
		}
	}

	//imprimirVector(personasPorSalvar);
	//imprimirVector(capacidadRestante);

	for(int i = 0; i < personasPorSalvar.size(); ++i){
		if(personasPorSalvar[i] > 0){
			cout << "quedan " << personasPorSalvar[i] << " personas en el punto " << i+1 <<endl;
			return false;
		}
	}

	for(int i = 0; i < capacidadRestante.size(); ++i){
		if(capacidadRestante[i] < 0){
			cout << "se excede en " << capacidadRestante[i] << " personas en el refugio " << i+1 << endl;
			return false;
		}	
	}
	
	return true;
}

int funcion_evaluacion(vector< vector< tuple<int, int> > > solucion){
	
	int tiempo [B] = {};
	int max = INT_MIN;
	vector<int> busesRestantes = busesEstacion; 

	for(int i = 0; i < solucion.size(); ++i){ //Bus i
		for(int j = 0; j < solucion[i].size(); ++j){ //Arco (p, s)
			if(j == 0){ //Sale de la estacion
				for(int w = 0; w < E; ++w){ //Buscamos la primera estacion disponible
					if(busesRestantes[w] > 0){
						tiempo[i] += distEstacionPtoEncuentro[w][get<0>(solucion[i][j])]; //Tiempo desde 
						busesRestantes[w]--;
						break;
					}
				}
				tiempo[i] += distPtoEncuentroRefugio[get<0>(solucion[i][j])][get<1>(solucion[i][j])];
			}
			else{
				tiempo[i] += distPtoEncuentroRefugio[get<0>(solucion[i][j])][get<1>(solucion[i][j-1])];
				tiempo[i] += distPtoEncuentroRefugio[get<0>(solucion[i][j])][get<1>(solucion[i][j])];
			}
		}
	}

	for(int i = 0; i < B; ++i){
		if(tiempo[i] > max)
			max = tiempo[i];
	}

	return max;
}

tuple<int, int> funcion_evaluacionV2(vector< vector< tuple<int, int> > > solucion){
	
	int tiempo [B] = {};
	int max = INT_MIN;
	vector<int> busesRestantes = busesEstacion; 

	for(int i = 0; i < solucion.size(); ++i){ //Bus i
		for(int j = 0; j < solucion[i].size(); ++j){ //Arco (p, s)
			if(j == 0){ //Sale de la estacion
				for(int w = 0; w < E; ++w){ //Buscamos la primera estacion disponible
					if(busesRestantes[w] > 0){
						tiempo[i] += distEstacionPtoEncuentro[w][get<0>(solucion[i][j])]; //Tiempo desde 
						busesRestantes[w]--;
						break;
					}
				}
				tiempo[i] += distPtoEncuentroRefugio[get<0>(solucion[i][j])][get<1>(solucion[i][j])];
			}
			else{
				tiempo[i] += distPtoEncuentroRefugio[get<0>(solucion[i][j])][get<1>(solucion[i][j-1])];
				tiempo[i] += distPtoEncuentroRefugio[get<0>(solucion[i][j])][get<1>(solucion[i][j])];
			}
		}
	}

	int bus;

	for(int i = 0; i < B; ++i){
		if(tiempo[i] > max){
			max = tiempo[i];
			bus = i;
		}
	}

	return make_tuple(max, bus);
}

vector<int> visita_necesaria(void){
  vector<int> visita_necesaria(P);
  for(int i = 0; i < P; ++i){
  	//cout << personasPE[i] << "/" << Q;
    visita_necesaria[i] = ceil((float)personasPE[i]/(float)Q);
    //cout << " " << visita_necesaria[i] << endl;
  }
  return visita_necesaria;
}


vector< vector< tuple<int, int> > > solucion_inicial(void){
	
	vector< vector< tuple<int, int> > > solInicial;
	vector<tuple<int, int> >  recorridoBus;

	vector<int> personasPorSalvar = personasPE;
	vector<int> capacidadRestante = capacidadS;

	vector<int> viajesNecesarios = visita_necesaria(); // Ve la cantidad de viajes necesarias a los refugios

	vector<int> finEvacuacion (B, 0);

	int refugio;
	int genteBus; 

	for(int i = 0; i < viajesNecesarios.size(); ++i){
		for(int j = 0; j < viajesNecesarios[i]; ++j){
			do{
				refugio = rand() % S; //elegimos un refugio aleatorio
				if(personasPorSalvar[i] <= capacidadRestante[refugio]){
					if(personasPorSalvar[i] < Q)
						genteBus = personasPorSalvar[i];
					else
						genteBus = Q;
					
				}
				else{ //personasPorSalvar[i] > capacidadRestante[refugio]
					if(capacidadRestante[refugio] >= Q)
						genteBus = Q;
					else
						genteBus = capacidadRestante[refugio];
				}
				
			}while(personasPorSalvar[i] <= 0 || capacidadRestante[refugio] <= 0);

			personasPorSalvar[i]-= genteBus;
			capacidadRestante[refugio]-= genteBus;

			recorridoBus.push_back(tuple<int, int>(i, refugio));

		}
	}

	vector< tuple<int, int> > bus;

	for(int i = 0; i < B; ++i){
		solInicial.push_back(bus);
	} 

	int randomSelect;

	while(recorridoBus.size() != 0){
		for(int i = B-1; i >= 0; --i){
			if(recorridoBus.size() != 0){			
				randomSelect = rand() % recorridoBus.size();	
				solInicial[i].push_back(recorridoBus[randomSelect]);
				recorridoBus.erase(recorridoBus.begin() + randomSelect);
			}
		}
	}

	return solInicial;

}

vector< vector< tuple<int, int> > >  movimiento(vector< vector< tuple<int, int> > > candidatoSolucion){
	vector< vector< tuple<int, int> > > vecino = candidatoSolucion;
	tuple<int, int> busMasLento = funcion_evaluacionV2(candidatoSolucion);

	if(get<1>(busMasLento) == get<0>(lastSWAP)){
		if(get<2>(lastSWAP) + 1 < candidatoSolucion[get<1>(busMasLento)].size()){
			swap(vecino[get<1>(busMasLento)][get<1>(lastSWAP)], vecino[get<1>(busMasLento)][get<2>(lastSWAP) + 1]);
			get<2>(lastSWAP)++;
		}
		else if (get<1>(lastSWAP) < candidatoSolucion[get<1>(busMasLento)].size() - 2){
			swap(vecino[get<1>(busMasLento)][get<1>(lastSWAP) + 1], vecino[get<1>(busMasLento)][get<1>(lastSWAP) + 2]);
			get<1>(lastSWAP)++;
			get<2>(lastSWAP) = get<1>(lastSWAP) + 1;
		}
		else{
			get<1>(lastSWAP) = 0;
			get<2>(lastSWAP) = 0;
		}
	}
	else{
		if(candidatoSolucion[get<1>(busMasLento)].size() == 1){//No hay nada que swapear
			get<0>(lastSWAP) = get<1>(busMasLento);
			get<1>(lastSWAP) = 0;
			get<2>(lastSWAP) = 0;	
		}
		else{
			swap(vecino[get<1>(busMasLento)][0], vecino[get<1>(busMasLento)][1]);
			get<0>(lastSWAP) = get<1>(busMasLento);
			get<1>(lastSWAP) = 0;
			get<2>(lastSWAP) = 1;	
		}

	}

	return vecino;	

}

int main (int argc, char *argv[]){

	int restart;
	bool verboso;

	switch(argc){
		case 1: 
			cout << "Archivo de instacia faltante" << endl;
			return 0;
		case 2: 
			restart = 1;
			verboso = false;
			break; 
		case 3:
			restart = atoi(argv[2]);
			verboso = false;
			break;
		default: 
			restart = atoi(argv[2]);
			verboso = true;
			break;
	}

	//cout << "restarts = " << restart << ", verboso = " << verboso << endl;

 	srand (time(NULL));

  	try	{
    	lectura_instacia(argv[1]);
  	} catch (int e){
    	cout << "Error al leer el archivo: " << e << endl;
  	}

  	if(verboso){
  		cout << endl;
  		cout << "Parametros de la instacia:" << endl;
  		cout << endl;
		cout << B << " : " << Q << endl;
		cout << E << " : ";

		for(int i=0; i<busesEstacion.size(); ++i)
		cout << busesEstacion[i] << "  ";
		cout << endl;

		cout << P << " : " << personasTotalPE << " : ";

		for(int i=0; i<personasPE.size(); ++i)
		cout << personasPE[i] << "  ";
		cout << endl;

		cout <<  S << " : " << capacidadTotalS << " : ";

		for(int i=0; i<capacidadS.size(); ++i)
		cout << capacidadS[i] << "  ";

		cout << endl;
		cout << endl;

		for(int i=0; i<distEstacionPtoEncuentro.size(); ++i){
		cout << i+1 << " : ";
		for(int j=0; j<distEstacionPtoEncuentro[i].size(); ++j){
		  cout << distEstacionPtoEncuentro[i][j] << "  ";
		}
		cout << endl;
		}

		cout << endl;

		for(int i=0; i<distPtoEncuentroRefugio.size(); ++i){
			cout << i+1 << " : ";
			for(int j=0; j<distPtoEncuentroRefugio[i].size(); ++j){
			  cout << distPtoEncuentroRefugio[i][j] << "  ";
			}
			cout << endl;
		}

		cout << endl;
		cout << endl;

		cout << "HILL CLIMBING:"<< endl;
		cout << "------------------------------------"<< endl;

		cout << "Numero de restart: " << restart << endl;
		cout << "------------------------------------"<< endl;

		vector< vector< tuple<int, int> > > mejorSolucion;
		vector< vector< tuple<int, int> > > vecino;
		vector< vector< tuple<int, int> > > mejorSolucionFinal;

		int tiempoMejorSolucionFinal = INT_MAX;
		tuple<int, int> tiempoMejorSolucion;
		tuple<int, int> tiempoVecino; 

		for(int i = 0; i < restart; ++i){
			
			lastSWAP  = make_tuple(-1, 0, 1);
			mejorSolucion = solucion_inicial();
			int contador = 0;

			while(true){ 
				cout << endl;
				cout << "Iteracion: " << contador << endl;
				cout << endl;
				cout << "Mejor Solucion:" << endl;
				cout << endl;
				tiempoMejorSolucion = funcion_evaluacionV2(mejorSolucion);
				imprimir_sol(mejorSolucion);
				//cout << "factible: "<< es_factible(mejorSolucion) << endl;
				cout << "Bus mas lento: " << get<1>(tiempoMejorSolucion) + 1 << " tiempo: "<< get<0>(tiempoMejorSolucion) << endl;
				cout << endl;
				cout << endl;
				cout << "Vecinos"<< endl;
				cout << "------------------------------------"<< endl;
				lastSWAP  = make_tuple(-1, 0, 1);

				do{
					cout << endl;
					//cout << "original: (ordinal->" << get<0>(lastSWAP) + 1 << ", " << get<1>(lastSWAP) << ", " << get<2>(lastSWAP) << ")" << endl; 
					vecino = movimiento(mejorSolucion);
					//cout << "nuevo: (ordinal->" << get<0>(lastSWAP)+ 1 << ", " << get<1>(lastSWAP) << ", " << get<2>(lastSWAP) << ")" << endl; 
					cout << "Movimiento: (" << get<0>(lastSWAP)+ 1 << ", " << get<1>(lastSWAP) << ", " << get<2>(lastSWAP) << ")" << endl;
					cout << endl;
					tiempoVecino = funcion_evaluacionV2(vecino);
					imprimir_sol(vecino);
					//cout << "factible: "<< es_factible(vecino) << endl;
					cout << "Bus mas lento: " << get<1>(tiempoVecino) + 1 << " tiempo: "<< get<0>(tiempoVecino) << endl;
					cout << "¿Es mejor?: ";
					if(get<0>(tiempoMejorSolucion) < get<0>(tiempoVecino))
						cout << "NO" << endl;
					
					else if(get<0>(tiempoMejorSolucion) > get<0>(tiempoVecino)){
						cout << "SI" << endl;
						cout << endl;
						cout << "_____________________________________" << endl; 
					}
					
					else
						cout << "NO" << endl; //No acepta soluciones iguales

					cout << endl;

				}while(get<0>(tiempoMejorSolucion) <= get<0>(tiempoVecino) && get<1>(lastSWAP) != get<2>(lastSWAP));


				/*cout << endl;
				cout << "sali con: " << endl;
				cout << "tiempo mejor solucion: " << get<0>(tiempoMejorSolucion) << endl;
				cout << "tiempo vecino " << get<0>(tiempoVecino) << endl;
				cout << "last swap 1 " << get<1>(lastSWAP) << endl;
				cout << "last swap 2 " << get<2>(lastSWAP) << endl;
				cout << endl;
				cout << endl;
				*/

				if(get<1>(lastSWAP) == get<2>(lastSWAP)) //No quedan vecinos por visitar
					break;
				
				mejorSolucion = vecino;
				contador++;
			
			}

			cout << endl;
			cout << "------------------------------------"<< endl;
			cout << "Nueva mejor solucion encontrada en la " << contador << " iteracion " << endl;
			cout << endl;
			imprimir_sol(mejorSolucion);
			//cout << "factible: "<< es_factible(mejorSolucion) << endl;
			cout << "Bus mas lento: " << get<1>(tiempoMejorSolucion) + 1 << " tiempo: "<< get<0>(tiempoMejorSolucion) << endl;
			cout << endl;

			tiempoMejorSolucion = funcion_evaluacionV2(mejorSolucion); 

			if(get<0>(tiempoMejorSolucion) <= tiempoMejorSolucionFinal){
  				mejorSolucionFinal = mejorSolucion;
  				tiempoMejorSolucionFinal = get<0>(tiempoMejorSolucion);
  			}

  		} 

		cout << endl;
		cout << endl;
		cout << "------------------------------------"<< endl;
		cout << "Mejor solucion encontrada luego de "<< restart << " restart"<< endl;
		imprimir_sol(mejorSolucionFinal);
		tiempoMejorSolucion = funcion_evaluacionV2(mejorSolucionFinal);
		//cout << "factible: "<< es_factible(mejorSolucionFinal) << endl;
		cout << "Bus mas lento: " << get<1>(tiempoMejorSolucion) + 1 << " tiempo: "<< get<0>(tiempoMejorSolucion) << endl;
		cout << endl;

		return 0;
		
  	}
  	else{

  		vector< vector< tuple<int, int> > > mejorSolucion;
		vector< vector< tuple<int, int> > > vecino;
		vector< vector< tuple<int, int> > > mejorSolucionFinal;

		int tiempoMejorSolucionFinal = INT_MAX;
		tuple<int, int> tiempoMejorSolucion;
		tuple<int, int> tiempoVecino; 

		for(int i = 0; i < restart; ++i){
		
			lastSWAP  = make_tuple(-1, 0, 1);
			mejorSolucion = solucion_inicial();
			int contador = 0;

			while(true){ 
				
				tiempoMejorSolucion = funcion_evaluacionV2(mejorSolucion);
				lastSWAP  = make_tuple(-1, 0, 1);

				do{
					vecino = movimiento(mejorSolucion);
					tiempoVecino = funcion_evaluacionV2(vecino);
					

				}while(get<0>(tiempoMejorSolucion) <= get<0>(tiempoVecino) && get<1>(lastSWAP) != get<2>(lastSWAP));

				if(get<1>(lastSWAP) == get<2>(lastSWAP)) //No quedan vecinos por visitar
					break;
				
				mejorSolucion = vecino;
				contador++;
			
			}
			
			tiempoMejorSolucion = funcion_evaluacionV2(mejorSolucion); 

			if(get<0>(tiempoMejorSolucion) <= tiempoMejorSolucionFinal){
  				mejorSolucionFinal = mejorSolucion;
  				tiempoMejorSolucionFinal = get<0>(tiempoMejorSolucion);
  			}

  		} 

		/*
		cout << endl;
		cout << endl;
		cout << "------------------------------------"<< endl;
		cout << "Mejor solucion FINAL es, encontrada en la " << endl;
		imprimir_sol(mejorSolucionFinal);
		tiempoMejorSolucion = funcion_evaluacionV2(mejorSolucionFinal);
		cout << "factible: "<< es_factible(mejorSolucionFinal) << endl;
		cout << "Bus mas lento: " << get<1>(tiempoMejorSolucion) + 1 << " tiempo: "<< get<0>(tiempoMejorSolucion) << endl;
		cout << endl;
		*/
		
		return 0;

  	}




}