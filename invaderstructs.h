//Definiendo estructuras del juego

//Jugador
struct player 
{
	int r,c;		//Posicion especifica del jugador (r=fila exacta y c=columna exacta)
	char ch;		//Imagen en consola
};

//Alien enemigo
struct alien 
{
	int r,c;		//Posicion especifica del alien
	int pr,pc;		//Posicion anterior del alien
	int alive; 		//1=vivo 0=destruido
	int showed;     //0=no ha aparecido    otro numero=orden en que va apareciendo
	int spawn_time;	//tiempo para aparecer en el mapa
	char direction; //'l'=left 'r'=right 'd'=down  's','S','z'=stop
	char pd;		//Direccion anterior (solo para aliens de tipo T)
	char ch;		//Imagen en consola
} t_alien;

//Disparo del jugador
struct shoot 
{
	int r,c;		//Posicion especifica del disparo
	int active; 	//1=activo 0=inactivo
	char ch;		//Imagen en consola
};

//Bomba del enemigo
struct bomb 
{
	int r,c;		//Posicion especifica de la bomba
	int active; 	//1=activa 0=inactiva
	int loop; 		//Usado para prevenir al alien que flashee cuando es arrojada
	char ch;		//Imagen en consola
};

//Parametros modificables del juego
struct options 
{
	int overall_speed,alien_speed,shots_speed,bombs_speed,bombchance,alien_amount,shots_amount,bombs_amount,ingame_aliens;
};
	
//TODAS LAS FUNCIONES DEL JUEGO DIVIDIDAS POR CATEGORIAS

//FUNCIONES QUE ADMINISTRAN MENUS
void main_menu();
void options_menu(struct options *settings);
void pause_menu();
void gameover(int win);
void game();

//FUNCIONES QUE DIBUJAN EN PANTALLA
void draw_main_menu();
void draw_options_menu();
void act_screen();
void move_aliens();
void move_shots();
void move_bombs();
void move_player();
void show_stats();

//OTRAS FUNCIONES
void write_input();
void init_config();
void init_config_e();
void init_config_h();
void init_config_x();
void starting_game();

