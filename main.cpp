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
// fuente: JOSE
// fecha: 24/06/2017

//! IMPORTANTE CAMBIAR FUENTE

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

/*
// version antigua
// fuente: https://stackoverflow.com/questions/25829143/c-trim-whitespace-from-a-string
string trim(const string& str){
    
    size_t first = str.find_first_not_of(' '); // size_t is an unsigned integer type of at least 16 bit 
    
    if (string::npos == first){
        return str;
    }
    
    size_t last = str.find_last_not_of(' ');
    
    return str.substr(first, (last - first + 1));
}
*/

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
	/*
	if(get<1>(busMasLento) == get<0>(lastSWAP)){
		if(get<1>(lastSWAP)] != candidatoSolucion.size() && get<2>(lastSWAP)] <= candidatoSolucion.size()){
			swap(vecino[get<1>(lastSWAP)], vecino[get<2>(lastSWAP)]);
			if(get<2>(lastSWAP) == candidatoSolucion.size()){
				get<1>(lastSWAP)++;
				get<1>(lastSWAP);
			}
		}
		
	}
	*/

}

/*
vector< vector< tuple<int, int> > > solucion_inicial_greedy(void){

	vector< vector< tuple<int, int> > > solInicial;
	vector<tuple<int, int> >  recorridoBus;
	
	vector<int> personasPorSalvar = personasPE;
	vector<int> capacidadRestante = capacidadS;

	int min;
	int genteBus;
	int puntoEncuento;
	int refugio; 

	vector<int> finEvacuacion (B, 0);

	while(personasPorSalvar != finEvacuacion){
		for(int i = 0; i < distPtoEncuentroRefugio.size(); ++i){

			if(personasPorSalvar[i] == 0){//No hay nadie por salvar por lo que se va a otro punto de encuentro
				continue; //Pasamos al siguiente punto de encuentro
			}

			min = INT_MAX;

			for(int j = 0; j < distPtoEncuentroRefugio[i].size(); ++j){
				if(distPtoEncuentroRefugio[i][j] < min && capacidadRestante[j] > 0){
					min = distPtoEncuentroRefugio[i][j];
					refugio = j;
				}
			}

			if(capacidadRestante[refugio] <= 0){
				continue;
			}
			
			if(personasPorSalvar[i] <= capacidadRestante[refugio]){
				if(personasPorSalvar[i] < Q){
					genteBus = personasPorSalvar[i];
				}
				else{
					genteBus = Q;
				}
			}
			else{
				if(capacidadRestante[refugio] > Q){
					genteBus = Q;
				}
				else{
					genteBus = capacidadRestante[refugio];
				}
			}

			
			personasPorSalvar[i]-= genteBus;
			capacidadRestante[refugio]-= genteBus;

			//cout << "punto de encuentro escogido: " << i << " gente por rescatar al salir: " << personasPorSalvar[i] << endl;
			//cout << "refugio escogido: " << refugio << " capacidad a la llegada: " << capacidadRestante[refugio] << endl;
			cout << endl;

			recorridoBus.push_back(tuple<int, int>(i, refugio));
			
		}
	}

	solInicial.push_back(recorridoBus);

	for(int i = 0; i < solInicial.size(); ++i){
		for(int j = 0; j < solInicial[i].size(); ++j){
			cout << "(" << get<0>(solInicial[i][j])+1 << "," << get<1>(solInicial[i][j])+1 << "), ";  
		}
	}

	cout << endl;
	
	return solInicial;
}
*/

int main (int argc, char *argv[]){

  srand (time(NULL));

  lectura_instacia(argv[1]);

  cout << "Los parametros son:" << endl;
  cout << "B = " << B << " : Q = " << Q << endl;
  cout << "E = " << E << " : ";

  for(int i=0; i<busesEstacion.size(); ++i)
    cout << busesEstacion[i] << "  ";
  cout << endl;

  cout << "P = " << P << " : PersonasTotalPE = " << personasTotalPE << " : ";

  for(int i=0; i<personasPE.size(); ++i)
    cout << personasPE[i] << "  ";
  cout << endl;

  cout << "S = " << S << " : capacidadTotalS = " << capacidadTotalS << " : ";

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

  /*
  cout << endl;
  //vector< vector< tuple<int, int> > > instaciaDePrueba = {{tuple<int, int>(1,2), tuple<int, int>(2,0)}, {tuple<int, int>(2,1), tuple<int, int>(0,1)}, {tuple<int, int>(1,1)}};
  vector< vector< tuple<int, int> > > instaciaDePrueba = {{tuple<int, int>(0,0), tuple<int, int>(2,1)}, {tuple<int, int>(1,0), tuple<int, int>(2,1)}, {tuple<int, int>(1,2), tuple<int, int>(1,1), tuple<int, int>(2,1)}};
  cout << "prueba de evaluacion 2.0;" << endl;
  cout << get<0>(funcion_evaluacionV2(instaciaDePrueba)) << " " << get<1>(funcion_evaluacionV2(instaciaDePrueba))+1 << endl; 
  imprimir_sol(instaciaDePrueba);
  cout << "Es factible: " << es_factible(instaciaDePrueba) << endl;
  vector< vector< tuple<int, int> > > candidato;
  cout << endl;
  cout << endl;
  cout << "Es factible:" << endl;
  candidato = solucion_inicial();
  cout << es_factible(candidato) << endl;
  cout << "Imprimiendo: " << endl;
  cout << endl;
  imprimir_sol(candidato);

  cout << "Pruebas: " << endl;
  cout << endl;

  for(int i = 0; i < 3; ++i){
  	candidato = solucion_inicial();
  	cout << "factible: "<< es_factible(candidato) << endl;
  	imprimir_sol(candidato);
  	cout << endl;
  }
*/
  cout << endl;
  cout << endl;

  /*
  vector< vector< tuple<int, int> > > candidato;
  tuple<int, int> maslargo; 

  for(int i = 0; i < 8; ++i){
  	candidato = solucion_inicial();
  	cout << "factible: "<< es_factible(candidato) << endl;
  	imprimir_sol(candidato);
  	maslargo = funcion_evaluacionV2(candidato);
  	cout << "Bus mas lento: " << get<1>(maslargo)+1 << " tiempo: "<< get<0>(maslargo) << endl;
  	cout << endl;
  	cout << endl;
  }

  */


 
  /*
  tuple<int, int> maslargo; 
  vector< vector< tuple<int, int> > > vecino = solucion_inicial();

  cout << "Movimientos: " << endl;
  for(int i = 0; i < 5; i++){
  	cout << i << ":  " << endl;
  	imprimir_sol(vecino);
  	cout << "factible: "<< es_factible(vecino) << endl;
  	maslargo = funcion_evaluacionV2(vecino);
  	cout << "Bus mas lento: " << get<1>(maslargo)+1 << " tiempo: "<< get<0>(maslargo) << endl;
  	cout << endl;
  	if(get<1>(lastSWAP) != get<2>(lastSWAP))
  		vecino = movimiento(vecino);
  	else
  		cout << "No se pueden realizar mas sawp: " << get<1>(lastSWAP) << "  " << get<2>(lastSWAP)<< " | " << endl;
  }
  */

  cout << "HILL CLIMBING:"<< endl;
  cout << "------------------------------------"<< endl;

  int restart = 1;

  cout << "Se utilizaran: " << restart << endl;
  cout << "------------------------------------"<< endl;

  vector< vector< tuple<int, int> > > mejorSolucion;
  vector< vector< tuple<int, int> > > vecino;
  vector< vector< tuple<int, int> > > mejorSolucionFinal;
 
  int rendimientoMejorSolucionFinal = INT_MIN;
  tuple<int, int> rendimientoMejorSolucion;
  tuple<int, int> rendimientoVecino; 


  for(int i = 0; i < restart; ++i){
  	
  	lastSWAP  = make_tuple(-1, 0, 1);
  	mejorSolucion = solucion_inicial();
  	int contador = 0;
	
	while(true){ 
		cout << endl;
		cout << "Solucion Actual:"<< endl;
		cout << contador << "." <<endl;
		rendimientoMejorSolucion = funcion_evaluacionV2(mejorSolucion);
  		imprimir_sol(mejorSolucion);
  		cout << "factible: "<< es_factible(mejorSolucion) << endl;
  		cout << "Bus mas lento: " << get<1>(rendimientoMejorSolucion) + 1 << " tiempo: "<< get<0>(rendimientoMejorSolucion) << endl;
  		cout << endl;
  		cout << endl;
  		cout << "Vecinos"<< endl;
  		cout << "------------------------------------"<< endl;
  		do{
  			vecino = movimiento(mejorSolucion);
  			rendimientoVecino = funcion_evaluacionV2(vecino);
  			imprimir_sol(vecino);
  			cout << "factible: "<< es_factible(vecino) << endl;
  			cout << "Bus mas lento: " << get<1>(rendimientoVecino) + 1 << " tiempo: "<< get<0>(rendimientoVecino) << endl;
  			cout << "¿Es mejor?: ";
  			if(get<0>(rendimientoMejorSolucion) > get<0>(rendimientoVecino)){
  				cout << "NO" << endl;
  			}

  			else if(get<0>(rendimientoMejorSolucion) < get<0>(rendimientoVecino)){
  				cout << "SI" << endl;
  			}
  			else
  				cout << "IGUAL pero se acepta" << endl;

  			cout << endl;

  		}while(get<0>(rendimientoMejorSolucion) > get<0>(rendimientoVecino) && get<1>(lastSWAP) != get<2>(lastSWAP));
  		
  		if(get<1>(lastSWAP) == get<2>(lastSWAP)){ //No quedan vecinos por visitar
  			break;
  		}

  		mejorSolucion = vecino;
  		contador++;



	} 

  	cout << endl;
  	cout << endl;
	cout << "------------------------------------"<< endl;
  	cout << "Mejor solucion en este ciclo, encontrada en la " << contador << " iteracion " << endl;
  	imprimir_sol(mejorSolucion);
  	cout << "factible: "<< es_factible(mejorSolucion) << endl;
  	cout << "Bus mas lento: " << get<1>(rendimientoMejorSolucion) + 1 << " tiempo: "<< get<0>(rendimientoMejorSolucion) << endl;
  	cout << endl;

  	rendimientoMejorSolucion = funcion_evaluacionV2(mejorSolucion);

  	if(get<0>(rendimientoMejorSolucion) >= rendimientoMejorSolucionFinal){
  		mejorSolucionFinal = mejorSolucion;
  		rendimientoMejorSolucionFinal = get<0>(rendimientoMejorSolucion);
  	}

  } 

  cout << endl;
  cout << endl;
  cout << "------------------------------------"<< endl;
  cout << "Mejor solucion FINAL es, encontrada en la " << endl;
  imprimir_sol(mejorSolucionFinal);
  rendimientoMejorSolucion = funcion_evaluacionV2(mejorSolucionFinal);
  cout << "factible: "<< es_factible(mejorSolucionFinal) << endl;
  cout << "Bus mas lento: " << get<1>(rendimientoMejorSolucion) + 1 << " tiempo: "<< get<0>(rendimientoMejorSolucion) << endl;
  cout << endl;

}

/*
#################################################
a
// basic file operations
#include <iostream>

using namespace std;

int main () {
  ofstream myfile;
  myfile.open ("example.txt");
  myfile << "Writing this to a file.\n";
  myfile.close();
  return 0;
}

// reading a text file
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

int main () {
  string line;
  ifstream myfile ("example.txt");
  if (myfile.is_open())
  {
    while ( getline (myfile,line) )
    {
      cout << line << '\n';
    }
    myfile.close();
  }

  else cout << "Unable to open file"; 

  return 0;
}

#include <stdlib.h> 

int leer_entradas(int argc, char **argv){
  //archivo con la instancia del problema
  archivo_configuracion=(char *)(argv[1]);
  
  //archivo donde escribir los resultados de la ejecucion
  archivo_resultados=(char *)(argv[2]);
  
  //SEMILLA
  semilla=atoi(argv[3]);

  //BUDGET
  Mr=atoi(argv[4]);
  if(debug)
    printf("Mr: %d\n", Mr);

  debug=atoi(argv[5]);

  return 1;
}

void leer_archivo_configuracion(void){

  char instancia[1000];
  //lectura de archivo de configuracion
  FILE *configuracion;
  char buf[100];
  int i,j;

  configuracion=fopen(archivo_configuracion, "r");
  int rt=fscanf(configuracion,"%s",buf);
  while(strcmp("NODE_COORD_SECTION",buf)!=0){
      if(strcmp("EDGE_WEIGHT_TYPE",buf)==0){
  rt=fscanf(configuracion,"%s",buf); //los dos puntos que estan separados
        rt=fscanf(configuracion,"%s",buf);
  if(strcmp("GEO",buf)==0) 
          tipodistancia=GEO;
  else if(strcmp("EUC_2D",buf)==0) 
    tipodistancia=EUC;
  buf[0]=0;
      }
      else if(strcmp("EDGE_WEIGHT_TYPE:",buf)==0){
  int rt=fscanf(configuracion,"%s",buf);
  if (strcmp("GEO",buf)==0) 
    tipodistancia=GEO;
  else if(strcmp("EUC_2D",buf)==0) 
    tipodistancia=EUC;
  buf[0]=0;
      }
      else if(strcmp("DIMENSION",buf)==0){
        rt=fscanf(configuracion,"%s",buf);//los dos puntos que estan separados
  rt=fscanf(configuracion,"%d",&Tinstancia);
  buf[0]=0;
      }
      else if(strcmp("DIMENSION:",buf)==0) {
  rt=fscanf(configuracion, "%d", &Tinstancia);
  buf[0]=0;
      }
      else if(strcmp("NAME",buf)==0)
        buf[0]=0;
      else if(strcmp("NAME:",buf)==0) 
  buf[0]=0;
      rt=fscanf(configuracion,"%s", buf);
    }

  if(strcmp("NODE_COORD_SECTION",buf)==0){
    if(Tinstancia==0){
      cout<<"ERROR: Problemas en la lectura del tamano de la instancia"<<endl;
      salir();
    }

    //Reserva de espacio para el arreglo de coordenadas y la matriz de distancia
    coordenadas=new coordenada[Tinstancia];
    distancia=new int*[Tinstancia];
    for( int i = 0 ; i < Tinstancia ; i++ )
      distancia[i] = new int [Tinstancia];
    //Lectura de las coordenadas de cada punto
    int id;
    float x,y;
    int contador=0;
    while(contador<Tinstancia){
      int rt=fscanf(configuracion,"%d %f %f\n",&id,&x,&y);
      coordenadas[id-1].x=x;
      coordenadas[id-1].y=y;
      contador++;
    }
  }


int main (int argc, char *argv[]){
  
  //lectura de parametros
  if(!leer_entradas(argc,argv)){
    cout<<"Problemas en la lectura de los parametros";
    exit(1);
  }

  //lectura de instancia
  restart=-1;
  iteracion=-1;
  leer_archivo_configuracion();
  solucion solucion_actual, candidata_solucion, mejor_candidata;

  //semilla aleatoria
  srand48 (semilla);
  bool optimo_local;
  
  crear_solucion_aleatoria(&solucion_actual);
  mejor_solucion=solucion_actual;
  if(debug) cout<<"Restart | Iteracion | Solucion-Actual \t\t\t\t |Vecindario \t\t\t\t\t  |Mejor-solucion"<<endl;   
  if(debug) cout<<"-------------------------------------------------------------------------------------";
  if(debug) cout<<"-------------------------------------------------------------------------------------"<<endl;
  for(restart=0;restart<Mr;restart++){
    optimo_local=false;
    iteracion=0;
    do{
      if(debug) cout<<"   "<<restart+1 <<" \t     "<<iteracion+1<<"\t\t"<<solucion_actual<<endl;
      //time_t ini_it=time(NULL);
      mejor_candidata=solucion_actual;
      for(int current=0;current<Tinstancia;current++){
          if(debug) cout<<"\t\t\t\t  \t\t\t";
          SWaP(&solucion_actual, &candidata_solucion, current, current+1);
          if(debug) cout<<"\t"<< candidata_solucion<<endl;
          if(candidata_solucion.aptitud < mejor_candidata.aptitud)
            mejor_candidata=candidata_solucion;
      }
      iteracion++;
      if(debug) getchar();
      if(mejor_candidata.aptitud<solucion_actual.aptitud) //Minimizacion
          solucion_actual=mejor_candidata;
      else
          optimo_local=true;
    }while(!optimo_local);
    if(solucion_actual.aptitud < mejor_solucion.aptitud)
        mejor_solucion = solucion_actual;
    if(debug) cout<<"\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t"<<mejor_solucion<<endl;
    crear_solucion_aleatoria(&solucion_actual);
    if(debug) getchar();
  }
  if(debug) cout<<"-------------------------------------------------------------------------------------"<<endl;
  salir();
}
*/