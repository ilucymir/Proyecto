#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <limits>

using namespace std;

class Celda {
public:
    bool mina;
    bool revelada;
    bool marcada;
    int minasVecinas;

    Celda() : mina(false), revelada(false), marcada(false), minasVecinas(0) {}
};

class Tablero {
    vector<vector<Celda>> celdas;
    int filas;
    int columnas;
    int minas;

public:
    void marcarCelda(int fila, int columna) {
        if (!celdas[fila][columna].revelada) {
            celdas[fila][columna].marcada = !celdas[fila][columna].marcada; // Alternar el estado
        }
    }

    bool esCeldaRevelada(int fila, int columna) const {
        return celdas[fila][columna].revelada;
    }

    Tablero(int filas, int columnas, int minas):filas(filas), columnas(columnas), minas(minas) {
        celdas.resize(filas, vector<Celda>(columnas));
        colocarMinas();
        calcularMinasVecinas();
    }

    void colocarMinas() {
        int colocadas = 0;
        while (colocadas < minas) {
            int fila = rand() % filas;
            int columna = rand() % columnas;
            if (!celdas[fila][columna].mina) {
                celdas[fila][columna].mina = true;
                colocadas++;
            }
        }
    }

    void calcularMinasVecinas() {
        for (int i = 0; i < filas; i++) {
            for (int j = 0; j < columnas; j++) {
                if (!celdas[i][j].mina) {
                    int contador = 0;
                    for (int k = -1; k <= 1; k++) {
                        for (int l = -1; l <= 1; l++) {
                            if (k == 0 && l == 0) continue;
                            int nuevaFila = i + k;
                            int nuevaColumna = j + l;
                            if (nuevaFila >= 0 && nuevaFila < filas && nuevaColumna >= 0 && nuevaColumna < columnas && celdas[nuevaFila][nuevaColumna].mina) {
                                contador++;
                            }
                        }
                    }
                    celdas[i][j].minasVecinas = contador;
                }
            }
        }
    }

    void mostrarTablero(bool revelarMinas = false) {
        cout << "  ";
        for (int j = 0; j < columnas; j++) {
            cout << j + 1 << " ";
        }
        cout << endl;

        for (int i = 0; i < filas; i++) {
            cout << i + 1 << " ";
            for (int j = 0; j < columnas; j++) {
                if (celdas[i][j].revelada) {
                    if (celdas[i][j].mina) {
                        cout << "* ";
                    } else {
                        cout << celdas[i][j].minasVecinas << " ";
                    }
                } else {
                    if (celdas[i][j].marcada) {
                        cout << "M "; // Mostrar 'M' para minas marcadas
                    } else {
                        cout << ". ";
                    }
                }
            }
            cout << endl;
        }
    }

    bool esMina(int fila, int columna) const {
        return celdas[fila][columna].mina;
    }

    void revelarCelda(int fila, int columna) {
        if (fila < 0 || fila >= filas || columna < 0 || columna >= columnas || celdas[fila][columna].revelada) {
            return;
        }
        celdas[fila][columna].revelada = true;
        if (celdas[fila][columna].minasVecinas == 0) {
            for (int k = -1; k <= 1; k++) {
                for (int l = -1; l <= 1; l++) {
                    if (k == 0 && l == 0) continue; // Evitar la celda actual
                    revelarCelda(fila + k, columna + l);
                }
            }
        }
    }

    int getFilas() const { return filas; }
    int getColumnas() const { return columnas; }
};






class Jugador {
protected:
    string nombre;
public:
    Jugador(string nombre) : nombre(nombre) {}

    virtual void jugar(Tablero& tablero) = 0;

    virtual ~Jugador() {}

    string getNombre() const {
        return nombre;
    }

};

class JugadorHumano : public Jugador {
public:
    JugadorHumano(string nombre) : Jugador(nombre) {}

    void jugar(Tablero &tablero) override {
        int fila, col;
        char accion;
        while (true) {
            cout << "Ingrese 'r' para revelar o 'm' para marcar (y luego fila y columna): ";
            cin >> accion;
            if (accion == 'r' || accion == 'm') {
                cout << "Ingrese la fila y columna (separados por espacio): ";
                if (cin >> fila >> col) {
                    fila--; // Ajustar a índice 0
                    col--;  // Ajustar a índice 0
                    if (fila >= 0 && fila < tablero.getFilas() && col >= 0 && col < tablero.getColumnas()) {
                        if (accion == 'r') {
                            tablero.revelarCelda(fila, col);
                        } else {
                            tablero.marcarCelda(fila, col);
                        }
                        break;
                    } else {
                        cout << "Coordenadas fuera de rango. Intente nuevamente." << endl;
                    }
                } else {
                    cin.clear(); // Limpiar el estado de error
                    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Ignorar entrada inválida
                    cout << "Entrada no válida. Intente nuevamente." << endl;
                }
            } else {
                cout << "Acción no válida. Intente nuevamente." << endl;
            }
        }
    }
};

class JugadorAutomatico : public Jugador {
public:
    JugadorAutomatico(string nombre) : Jugador(nombre) {}

    void jugar(Tablero &tablero) override {
        int fila, col;
        do {
            fila = rand() % tablero.getFilas();
            col = rand() % tablero.getColumnas();
        } while (tablero.esCeldaRevelada(fila, col)); // Cambiado aquí
        cout << nombre << " revela la celda (" << fila + 1 << ", " << col + 1 << ")." << endl;
        tablero.revelarCelda(fila, col);
    }
};


class Partida {
public:
    string jugador;
    string resultado; // "Victoria" o "Derrota"
    string duracion;  // Duración de la partida
    time_t fecha;

    Partida(string jugador, string resultado, string duracion, time_t fecha)
        : jugador(jugador), resultado(resultado), duracion(duracion), fecha(fecha) {}

    friend ostream& operator<<(ostream& os, const Partida& partida) {
        char buffer[80];
        struct tm* timeinfo;
        timeinfo = localtime(&partida.fecha);
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);

        os << partida.jugador << " | Resultado: " << partida.resultado
           << " | Duración: " << partida.duracion
           << " | Fecha: " << buffer << endl;

        return os;
    }
};

class Historial {
    vector<Partida> partidas;
public:
    // Sobrecarga del operador ==
    bool operator==(const string& nombreJugador) const {
        for (const auto& partida : partidas) {
            if (partida.jugador == nombreJugador) {
                return true; // El jugador ya existe
            }
        }
        return false;
    }
    // Cargar historial desde un archivo
    void cargarHistorial(const string& archivo) {
        ifstream file(archivo);
        if (file.is_open()) {
            string linea;
            while (getline(file, linea)) {
                stringstream ss(linea);
                string jugador, resultado, duracion;
                time_t fecha;
                getline(ss, jugador, '|');
                getline(ss, resultado, '|');
                getline(ss, duracion, '|');
                ss >> fecha;

                resultado.erase(0, resultado.find_first_not_of(" "));
                resultado.erase(resultado.find_last_not_of(" ") + 1);

                partidas.push_back(Partida(jugador, resultado, duracion, fecha));
            }
            file.close();
            cout << "Historial cargado. Total de partidas: " << partidas.size() << endl; // Depuración
        } else {
            cout << "No se pudo abrir el archivo " << archivo << endl;
        }
    }


    // Guardar historial en un archivo
    void guardarHistorial(const string& archivo) {
        ofstream file(archivo);
        if (file.is_open()) {
            for (const auto& partida : partidas) {
                file << partida.jugador << '|'
                     << partida.resultado << '|'
                     << partida.duracion << '|'
                     << partida.fecha << endl;
            }
            file.close();
        }
    }

    // Mostrar historial completo
    void mostrarHistorialTotal() const {
        cout << *this; // Usamos la sobrecarga del operador << de `Historial`
    }

    vector<string> obtenerListaJugadores() const {
        vector<string> jugadores;
        for (const auto& partida : partidas) {
            // Convertir el nombre a minúsculas para garantizar unicidad sin importar mayúsculas/minúsculas
            string nombreNormalizado = partida.jugador;
            transform(nombreNormalizado.begin(), nombreNormalizado.end(), nombreNormalizado.begin(), ::tolower);

            // Agregar solo si el nombre normalizado no existe ya
            if (find_if(jugadores.begin(), jugadores.end(), [&](const string& existente) {
                    string existenteNormalizado = existente;
                    transform(existenteNormalizado.begin(), existenteNormalizado.end(), existenteNormalizado.begin(), ::tolower);
                    return existenteNormalizado == nombreNormalizado;
                }) == jugadores.end()) {
                jugadores.push_back(partida.jugador);
                }
        }
        return jugadores;
    }

    void mostrarHistorialPorJugador() const {
        vector<string> jugadores = obtenerListaJugadores();
        if (jugadores.empty()) {
            cout << "No hay jugadores en el historial." << endl;
            return;
        }

        cout << "Jugadores disponibles:" << endl;
        for (size_t i = 0; i < jugadores.size(); ++i) {
            cout << i + 1 << ". " << jugadores[i] << endl;
        }

        cout << "Seleccione un jugador por número: ";
        int opcion;
        cin >> opcion;

        if (opcion > 0 && opcion <= jugadores.size()) {
            string nombreJugador = jugadores[opcion - 1];
            int victorias = 0, derrotas = 0;

            for (const auto& partida : partidas) {
                if (partida.jugador == nombreJugador) {
                    // Lógica de conteo de resultados
                    if (partida.resultado == "Victoria") {
                        victorias++;
                    } else if (partida.resultado == "Derrota") {
                        derrotas++;
                    }
                }
            }

            // Mostrar resultados
            cout << "Jugador: " << nombreJugador << endl;
            cout << "Victorias: " << victorias << endl;
            cout << "Derrotas: " << derrotas << endl;

            // Mostrar detalles de partidas
            for (const auto& partida : partidas) {
                if (partida.jugador == nombreJugador) {
                    cout << partida; // Se usa el operador << sobrecargado para mostrar la partida
                }
            }
        } else {
            cout << "Opción no válida. Intente nuevamente." << endl;
        }
    }




    // Agregar una nueva partida al historial
    void agregarPartida(const string& jugador, const string& resultado, const string& duracion) {
        time_t now = time(0);
        // Verificar si el jugador ya existe
        bool jugadorExistente = false;
        for (auto& partida : partidas) {
            if (partida.jugador == jugador) {
                jugadorExistente = true;
                break;
            }
        }
        // Si el jugador no existe, agregar la nueva partida
        if (!jugadorExistente) {
            partidas.push_back(Partida(jugador, resultado, duracion, now));
        } else {
            // Si el jugador ya existe, simplemente agregar la nueva partida
            partidas.push_back(Partida(jugador, resultado, duracion, now));
        }
        cout << "Partida agregada: " << jugador << " | " << resultado << " | " << duracion << endl;
    }

    void borrarHistorial() {
        partidas.clear(); // Vaciar la lista de partidas en memoria
        ofstream file("historial.txt", ios::trunc); // Abrir el archivo en modo truncar
        if (file.is_open()) {
            cout << "El historial ha sido borrado exitosamente." << endl;
            file.close();
        } else {
            cout << "Error al intentar borrar el historial." << endl;
        }
    }

    friend ostream& operator<<(ostream& os, const Historial& historial) {
        if (historial.partidas.empty()) {
            os << "No hay partidas en el historial." << endl;
        } else {
            for (const auto& partida : historial.partidas) {
                os << partida;
            }
        }
        return os;
    }

};

class JuegoBuscaminas {
    Tablero tablero;
    Jugador *jugador1;
    Jugador *jugador2;
    int turno; // 0 para jugador1, 1 para jugador2
    Historial historial; // Miembro no estático

public:
    JuegoBuscaminas(int filas, int columnas, int minas, Jugador *j1, Jugador *j2)
        : tablero(filas, columnas, minas), jugador1(j1), jugador2(j2), turno(0) {
        historial.cargarHistorial("historial.txt"); // Cargar historial al iniciar
    }

    void jugar() {
        bool juegoTerminado = false;
        time_t inicio = time(0); // Tiempo inicial del juego

        while (!juegoTerminado) {
            tablero.mostrarTablero();

            // Usar un puntero único para el jugador actual
            Jugador* jugadorActual = (turno == 0) ? jugador1 : jugador2;
            cout << "Turno de: " << jugadorActual->getNombre() << endl;
            jugadorActual->jugar(tablero);

            if (verificarFinJuego(jugadorActual)) {
                time_t fin = time(0); // Tiempo final del juego
                int duracion = static_cast<int>(difftime(fin, inicio));

                // Determinar perdedor y ganador basado en el turno
                Jugador* perdedor = jugadorActual;
                Jugador* ganador = (turno == 0) ? jugador2 : jugador1;

                guardarResultados(perdedor, ganador, duracion, "Derrota", "Victoria");
                juegoTerminado = true;
                break; // Terminar inmediatamente
            }

            if (verificarVictoria(jugadorActual)) {
                time_t fin = time(0); // Tiempo final del juego
                int duracion = static_cast<int>(difftime(fin, inicio));

                // Determinar ganador y perdedor basado en el turno
                Jugador* ganador = jugadorActual;
                Jugador* perdedor = (turno == 0) ? jugador2 : jugador1;

                guardarResultados(perdedor, ganador, duracion, "Derrota", "Victoria");
                juegoTerminado = true;
                break;
            }

            // Cambiar de turno
            turno = 1 - turno;
        }

        historial.guardarHistorial("historial.txt"); // Guardar historial al finalizar
        volverAlMenu();  // Llamar a la función que maneja las opciones de menú
    }

    void guardarResultados(Jugador* perdedor, Jugador* ganador, int duracion, string resultadoPerdedor, string resultadoGanador) {
        int minutos = duracion / 60;
        int segundos = duracion % 60;
        cout << "Duración de la partida: " << minutos << " minutos con " << segundos << " segundos." << endl;
        historial.agregarPartida(perdedor->getNombre(), resultadoPerdedor, to_string(minutos) + " minutos y " + to_string(segundos) + " segundos");
        historial.agregarPartida(ganador->getNombre(), resultadoGanador, to_string(minutos) + " minutos y " + to_string(segundos) + " segundos");
    }

    bool verificarFinJuego(Jugador *jugador) {
        for (int i = 0; i < tablero.getFilas(); i++) {
            for (int j = 0; j < tablero.getColumnas(); j++) {
                if (tablero.esMina(i, j) && tablero.esCeldaRevelada(i, j)) { // Cambiado aquí
                    cout << jugador->getNombre() << " ha perdido al revelar una mina." << endl;
                    tablero.mostrarTablero(true);
                    return true;
                }
            }
        }
        return false;
    }

    bool verificarVictoria(Jugador *jugador) {
        for (int i = 0; i < tablero.getFilas(); i++) {
            for (int j = 0; j < tablero.getColumnas(); j++) {
                if (!tablero.esCeldaRevelada(i, j) && !tablero.esMina(i, j)) { // Cambiado aquí
                    return false; // Hay celdas sin revelar que no son minas
                }
            }
        }
        cout << jugador->getNombre() << " ha ganado al revelar todas las celdas sin minas." << endl;
        return true;
    }

    void volverAlMenu() {
        Historial historial;
        historial.cargarHistorial("historial.txt");
        int opcion;
        while (true) {
            cout << "Historial de Partidas" << endl;
            cout << "1. Ver historial completo" << endl;
            cout << "2. Ver historial por jugador" << endl;
            cout << "3. Regresar al menu principal" << endl;
            cout << "Seleccione una opcion: ";
            cin >> opcion;

            if (opcion == 1) {
                historial.mostrarHistorialTotal();  // Ya no es estático
            } else if (opcion == 2) {
                historial.mostrarHistorialPorJugador();  // Ya no es estático
            } else if (opcion == 3) {
                return; // Regresar al menú principal
            } else {
                cout << "Opcion no valida. Intente nuevamente." << endl;
            }
        }
    }

    static void menuHistorial() {
        Historial historial;
        historial.cargarHistorial("historial.txt");

        int opcion;
        while (true) {
            cout << "Historial de Partidas" << endl;
            cout << "1. Ver historial completo" << endl;
            cout << "2. Ver historial por jugador" << endl;
            cout << "3. Borrar historial" << endl;
            cout << "4. Regresar al menú principal" << endl;
            cout << "Seleccione una opción: ";
            cin >> opcion;

            if (opcion == 1) {
                historial.mostrarHistorialTotal();
            } else if (opcion == 2) {
                historial.mostrarHistorialPorJugador();
            } else if (opcion == 3) {
                cout << "¿Está seguro de que desea borrar todo el historial? (s/n): ";
                char confirmacion;
                cin >> confirmacion;
                if (confirmacion == 's' || confirmacion == 'S') {
                    historial.borrarHistorial();
                } else {
                    cout << "Operación cancelada." << endl;
                }
            } else if (opcion == 4) {
                return; // Regresar al menú principal
            } else {
                cout << "Opción no válida. Intente nuevamente." << endl;
            }
        }
    }

	static void menu(){
        int opcion;
        cout << "Bienvenido al juego de Buscaminas!" << endl;
        cout << "1. Partida normal" << endl;
        cout << "2. Juego Automatico" << endl;
        cout << "3. Ver historial" << endl;
        cout << "Seleccione una opcion: ";
        cin >> opcion;
        if (opcion == 1) {
            string nombre1, nombre2;
            cout << "Ingrese el nombre del Jugador 1: ";
            cin >> nombre1;
            cout << "Ingrese el nombre del Jugador 2: ";
            cin >> nombre2;
            JugadorHumano jugador1(nombre1);
            JugadorHumano jugador2(nombre2);
            JuegoBuscaminas juego(8, 8, 10, &jugador1, &jugador2);
            juego.jugar();
        } else if (opcion == 2) {
            string nombre, dificultad;
            cout << "Ingrese su nombre: ";
            cin >> nombre;
            cout << "Seleccione la dificultad (facil, medio, dificil): ";
            cin >> dificultad;
            JugadorHumano jugador(nombre);
            JugadorAutomatico computadora("Computadora");
            JuegoBuscaminas juego(8, 8, 10, &jugador, &computadora);
            juego.jugar();
        } else if (opcion == 3) {
            menuHistorial();  // Ya no es estático
        }
    }

};


int main() {
    srand(static_cast<unsigned int>(time(0)));
    while (true) {
      JuegoBuscaminas::menu();
      }

    return 0;
}

