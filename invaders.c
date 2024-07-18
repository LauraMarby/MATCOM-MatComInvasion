#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <curses.h>
#include <time.h>
#include <pthread.h>
#include <signal.h>
#include "invaderstructs.h"

//--------------------------------------------------------DECLARANDO VARIABLES Y ESTRUCTURAS GLOBALES----------------------------------------------------------
struct player tank;
struct alien *aliens;
struct alien *ingame_aliens;
int *alien_out_time;
struct shoot shot[1000];
struct bomb bomb[1000];
struct options settings;
unsigned int input, loops, currentshots, currentbombs, currentaliens;
int score, win, difficult = 1, current_alien, current_showed_alien;
char tellscore[30];

// Declarando exclusiones mutuas
pthread_mutex_t win_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t player_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t alien_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t shot_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t bomb_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t score_mutex = PTHREAD_MUTEX_INITIALIZER;   // Este es el mutex del score que se va mostrando en pantalla
pthread_mutex_t pointer_mutex = PTHREAD_MUTEX_INITIALIZER; // Este es el mutex del puntero de escritura de curses (biblioteca de dibujar en consola)

//----------------------------------------------------------IMPLEMENTANDO FUNCIONES DE MENUS-------------------------------------------------------------------
// Funcion que maneja el menu principal del juego
void main_menu()
{
   // Detener procesos anteriores en caso de haberlos
   clear();
   // Variable de opcion
   char option;
   // Dibujar pantalla de menu
   draw_main_menu();
   while (1)
   {
      // Eleccion de opcion
      option = getch();
      if (option == '3')
      {
         // Cerrar biblioteca y salir del juego
         endwin();
         exit(0);
      }
      else if (option == '1')
      {
         // Limpiar pantalla, inicializar juego y cuando termine el juego limpiar pantalla y cerrar
         clear();
         starting_game();
         game();
         clear();
         move(0, 0);
         endwin();
         exit(0);
      }
      else if (option == '2')
      {
         // Abrir menu de opciones y cuando termine volver a dibujar el menu principal
         options_menu(&settings);
         draw_main_menu();
      }
      else if (option == ' ')
      {
         // Cambiar dificultad al pulsar espacio y despues dibujar de nuevo el menu para actualizar la dificultad
         if (difficult == 0)
         {
            init_config();
            difficult = 1;
            draw_main_menu();
         }
         else if (difficult == 1)
         {
            init_config_h();
            difficult = 2;
            draw_main_menu();
         }
         else if (difficult == 2)
         {
            init_config_x();
            difficult = 3;
            draw_main_menu();
         }
         else if (difficult == 3)
         {
            init_config_e();
            difficult = 0;
            draw_main_menu();
         }
         else if (difficult == 4)
         {
            init_config_e();
            difficult = 0;
            draw_main_menu();
         }
      }
   }
}

// Funcion que maneja el menu de opciones del juego
void options_menu(struct options *settings)
{
   // Establecer variables de seleccion
   char option, buf[30];
   int new;
   // Dibujar menu
   draw_options_menu();
   // Ciclo de seleccion
   while (1)
   {
      // Seleccion
      move(11, 21);
      option = getch();
      move(10, 0);
      deleteln();
      move(11, 0);
      deleteln();
      move(12, 0);
      deleteln();
      // Opciones
      if (option == '1')
      {
         sprintf(buf, "Valor actual: %d\n", settings->overall_speed);
         move(10, 0);
         addstr(buf);
         move(11, 0);
         addstr("Introduzca nuevo valor: ");
         move(11, 23);
         refresh();
         getch();
         getstr(buf);
         new = atoi(buf);
         // Verificando si el valor es valido
         if (new < 0)
         {
            move(14, 0);
            addstr("ERROR: Valor inválido");
         }
         else
         {
            settings->overall_speed = new;
            move(14, 0);
            addstr("VALOR CAMBIADO SATISFACTORIAMENTE");
            difficult = 4;
         }
         refresh();
         sleep(2);
         draw_options_menu();
      }
      else if (option == '2')
      {
         sprintf(buf, "Valor actual: %d\n", settings->alien_speed);
         move(10, 0);
         addstr(buf);
         move(11, 0);
         addstr("Introduzca nuevo valor: ");
         move(11, 23);
         refresh();
         getch();
         getstr(buf);
         new = atoi(buf);
         // Verificando si el valor es valido
         if (new < 0)
         {
            move(14, 0);
            addstr("ERROR: Valor inválido");
         }
         else
         {
            settings->alien_speed = new;
            move(14, 0);
            addstr("VALOR CAMBIADO SATISFACTORIAMENTE");
            difficult = 4;
         }
         refresh();
         sleep(2);
         draw_options_menu();
      }
      else if (option == '3')
      {
         sprintf(buf, "Valor actual: %d\n", settings->shots_speed);
         move(10, 0);
         addstr(buf);
         move(11, 0);
         addstr("Introduzca nuevo valor: ");
         move(11, 23);
         refresh();
         getch();
         getstr(buf);
         new = atoi(buf);
         // Verificando si el valor es valido
         if (new < 0)
         {
            move(14, 0);
            addstr("ERROR: Valor inválido");
         }
         else
         {
            settings->shots_speed = new;
            move(14, 0);
            addstr("VALOR CAMBIADO SATISFACTORIAMENTE");
            difficult = 4;
         }
         refresh();
         sleep(2);
         draw_options_menu();
      }
      else if (option == '4')
      {
         sprintf(buf, "Valor actual: %d\n", settings->bombs_speed);
         move(10, 0);
         addstr(buf);
         move(11, 0);
         addstr("Introduzca nuevo valor: ");
         move(11, 23);
         refresh();
         getch();
         getstr(buf);
         new = atoi(buf);
         // Verificando si el valor es valido
         if (new < 0)
         {
            move(14, 0);
            addstr("ERROR: Valor inválido");
         }
         else
         {
            settings->bombs_speed = new;
            move(14, 0);
            addstr("VALOR CAMBIADO SATISFACTORIAMENTE");
            difficult = 4;
         }
         refresh();
         sleep(2);
         draw_options_menu();
      }
      else if (option == '5')
      {
         sprintf(buf, "Valor actual: %d\n", settings->bombchance);
         move(10, 0);
         addstr(buf);
         move(11, 0);
         addstr("Introduzca nuevo valor: ");
         move(11, 23);
         refresh();
         getch();
         getstr(buf);
         new = atoi(buf);
         // Verificando si el valor es valido
         if (new < 0)
         {
            move(14, 0);
            addstr("ERROR: Valor inválido");
         }
         else
         {
            settings->bombchance = new;
            move(14, 0);
            addstr("VALOR CAMBIADO SATISFACTORIAMENTE");
            difficult = 4;
         }
         refresh();
         sleep(2);
         draw_options_menu();
      }
      else if (option == '6')
      {
         sprintf(buf, "Valor actual: %d\n", settings->alien_amount);
         move(10, 0);
         addstr(buf);
         move(11, 0);
         addstr("Introduzca nuevo valor: ");
         move(11, 23);
         refresh();
         getch();
         getstr(buf);
         new = atoi(buf);
         // Verificando si el valor es valido
         if (new < 0 || new > 1000)
         {
            move(14, 0);
            addstr("ERROR: Valor inválido");
         }
         else
         {
            settings->alien_amount = new;
            move(14, 0);
            addstr("VALOR CAMBIADO SATISFACTORIAMENTE");
            difficult = 4;
         }
         refresh();
         sleep(2);
         draw_options_menu();
      }
      else if (option == '7')
      {
         sprintf(buf, "Valor actual: %d\n", settings->shots_amount);
         move(10, 0);
         addstr(buf);
         move(11, 0);
         addstr("Introduzca nuevo valor: ");
         move(11, 23);
         refresh();
         getch();
         getstr(buf);
         new = atoi(buf);
         // Verificando si el valor es valido
         if (new < 0 || new > 1000)
         {
            move(14, 0);
            addstr("ERROR: Valor inválido");
         }
         else
         {
            settings->shots_amount = new;
            move(14, 0);
            addstr("VALOR CAMBIADO SATISFACTORIAMENTE");
            difficult = 4;
         }
         refresh();
         sleep(2);
         draw_options_menu();
      }
      else if (option == '8')
      {
         sprintf(buf, "Valor actual: %d\n", settings->bombs_amount);
         move(10, 0);
         addstr(buf);
         move(11, 0);
         addstr("Introduzca nuevo valor: ");
         move(11, 23);
         refresh();
         getch();
         getstr(buf);
         new = atoi(buf);
         // Verificando si el valor es valido
         if (new < 0 || new > 1000)
         {
            move(14, 0);
            addstr("ERROR: Valor inválido");
         }
         else
         {
            settings->bombs_amount = new;
            move(14, 0);
            addstr("VALOR CAMBIADO SATISFACTORIAMENTE");
            difficult = 4;
         }
         refresh();
         sleep(2);
         draw_options_menu();
      }
      else if (option == '9')
      {
         sprintf(buf, "Valor actual: %d\n", settings->ingame_aliens);
         move(10, 0);
         addstr(buf);
         move(11, 0);
         addstr("Introduzca nuevo valor: ");
         move(11, 23);
         refresh();
         getch();
         getstr(buf);
         new = atoi(buf);
         // Verificando si el valor es valido
         if (new < 0 || new > 1000)
         {
            move(14, 0);
            addstr("ERROR: Valor inválido");
         }
         else
         {
            settings->ingame_aliens = new;
            move(14, 0);
            addstr("VALOR CAMBIADO SATISFACTORIAMENTE");
            difficult = 4;
         }
         refresh();
         sleep(2);
         draw_options_menu();
      }
      else if (option == 'r' || option == 'R')
      {
         break;
      }
      else if (option == 'q' || option == 'Q')
      {
         endwin();
         exit(0);
      }
   }
   clear();
   noecho();
   cbreak();
   nodelay(stdscr, 1);
}

// Funcion que maneja el menu de pausa del juego
void pause_menu()
{
   // Configurando biblioteca
   nodelay(stdscr, 0);
   // Dibujando menu
   move((LINES)-3, 0);
   addstr("EL JUEGO ESTA EN PAUSA... PRESIONE CUALQUIER TECLA PARA CONTINUAR");
   refresh();
   // Al pulsar una tecla, limpia la pantalla
   getch();
   clear();
   // Reconfigurando biblioteca a su estado anterior
   nodelay(stdscr, 1);
   // Dibujar estadisticas del juego y continuar
   show_stats();
}

// Esta funcion muestra la pantalla de victoria/derrota
void gameover(int win)
{
   pthread_mutex_lock(&pointer_mutex);
   // Configurando biblioteca
   nodelay(stdscr, 0);
   // Mostrando pantalla en dependencia de la condicion de victoria/derrota
   clear();
   if (win == 0)
   {
      move((LINES / 2) - 1, 0);
      addstr("HAS PERDIDO LA PARTIDA: UNA BOMBA TE HA IMPACTADO");
      move((LINES / 2), (0));
      addstr("PRESIONA CUALQUIER TECLA PARA SALIR DEL JUEGO");
   }
   else if (win == 1)
   {
      move((LINES / 2) - 1, (0));
      addstr("HAS GANADO LA PARTIDA!!!");
      move((LINES / 2), (0));
      addstr("PRESIONA CUALQUIER TECLA PARA SALIR DEL JUEGO");
   }
   else if (win == 2)
   {
      move((LINES / 2) - 1, 0);
      addstr("HAS PERDIDO LA PARTIDA: LOS ALIENS HAN LLEGADO A LA TIERRA");
      move((LINES / 2), (0));
      addstr("PRESIONA CUALQUIER TECLA PARA SALIR DEL JUEGO");
   }
   else if (win == 3)
   {
      move((LINES / 2) - 1, 0);
      addstr("HAS PERDIDO LA PARTIDA: HAS RENUNCIADO A TU MISION");
      move((LINES / 2), (0));
      addstr("PRESIONA CUALQUIER TECLA PARA SALIR DEL JUEGO");
   }
   move(0, COLS - 1);
   refresh();
   sleep(2); // Esperar dos segundos antes de cerrar
   getch();
   pthread_mutex_unlock(&pointer_mutex);
}

//--------------------------------------------------------IMPLEMENTANDO FUNCIONES DE PANTALLA------------------------------------------------------------------

// Funcion que muestra la pantalla de estadisticas del juego
void show_stats()
{
   // Mostrar titulo, score, opciones...
   pthread_mutex_lock(&pointer_mutex);
   pthread_mutex_lock(&score_mutex);
   move(0, 0);
   move(0, (COLS / 2) - 6);
   addstr("--MATCOM INVASION--");
   move(0, 1);
   addstr("SCORE: ");
   move(0, 15);
   addstr("REMAINING: ");
   move(0, COLS - 21);
   addstr("P = Pausa  Q = Salir");
   move(0, COLS - 1);
   sprintf(tellscore, "%d", score);
   move(0, 8);
   addstr(tellscore);
   sprintf(tellscore, "%d", currentaliens);
   move(0, 26);
   addstr(tellscore);
   addstr("-");
   move(0, 0);
   refresh();
   pthread_mutex_unlock(&score_mutex);
   pthread_mutex_unlock(&pointer_mutex);
}

// Funcion que mueve al jugador
void move_player()
{
   pthread_mutex_lock(&player_mutex);
   pthread_mutex_lock(&pointer_mutex);
   //  Pone el puntero en la posicion actualizada del jugador y lo dibuja
   move(tank.r, tank.c);
   addch(tank.ch);
   // Verificar posicion valida de jugador
   if (tank.c > COLS - 1)
   {
      tank.c = COLS - 1;
   }
   else if (tank.c < 0)
   {
      tank.c = 0;
   }
   pthread_mutex_unlock(&pointer_mutex);
   pthread_mutex_unlock(&player_mutex);
}

// Funcion que mueve las bombas de aliens
void move_bombs()
{
   pthread_mutex_lock(&bomb_mutex);
   pthread_mutex_lock(&pointer_mutex);
   if (loops % settings.bombs_speed == 0) // La variable loops define la velocidad a la que va el juego.
   {
      for (int i = 0; i < settings.bombs_amount; ++i)
      {
         if (bomb[i].active == 1)
         {
            // Si la bomba esta activa, muevela de posicion
            if (bomb[i].r < LINES)
            {
               // Poniendo en blanco la posicion anterior
               if (bomb[i].loop != 0)
               {
                  move(bomb[i].r - 1, bomb[i].c);
                  addch(' ');
               }
               else
               {
                  ++bomb[i].loop;
               }
               // Dibujando bomba en nueva posicion
               move(bomb[i].r, bomb[i].c);
               addch(bomb[i].ch);
               ++bomb[i].r;
            }
            // Elimina la bomba si llego al fondo del campo
            else
            {
               bomb[i].active = 0;
               bomb[i].loop = 0;
               --currentbombs;
               move(bomb[i].r - 1, bomb[i].c);
               addch(' ');
            }
         }
      }
   }
   pthread_mutex_unlock(&pointer_mutex);
   pthread_mutex_unlock(&bomb_mutex);
}

// Funcion que mueve los disparos del jugador
void move_shots()
{
   pthread_mutex_lock(&alien_mutex);
   pthread_mutex_lock(&pointer_mutex);
   pthread_mutex_lock(&score_mutex);
   pthread_mutex_lock(&shot_mutex);
   if (loops % settings.shots_speed == 0)
   {
      for (int i = 0; i < settings.shots_amount; ++i)
      {
         // Si el disparo del jugador esta activo
         if (shot[i].active == 1)
         {
            if (shot[i].r > 0)
            {
               if (shot[i].r < LINES - 2)
               {
                  // Eliminar disparo de la posicion anterior
                  move(shot[i].r + 1, shot[i].c);
                  addch(' ');
               }
               for (int j = 0; j < settings.ingame_aliens; ++j)
               {
                  if (ingame_aliens[j].alive == 1 && ingame_aliens[j].r == shot[i].r && ingame_aliens[j].pc == shot[i].c)
                  {
                     // Si el disparo impacto a un enemigo, borra el disparo, mata al alien y aumenta el score
                     score += 20;
                     ingame_aliens[j].alive = 0;
                     shot[i].active = 0;
                     --currentshots;
                     --currentaliens;
                     move(ingame_aliens[j].pr, ingame_aliens[j].pc);
                     addch(' ');
                     break;
                  }
               }
               if (shot[i].active == 1)
               {
                  // Dibujar disparo en la posicion siguiente (si esta activo)
                  move(shot[i].r, shot[i].c);
                  addch(shot[i].ch);
                  --shot[i].r;
               }
            }
            else
            {
               // Borrar disparo si llego al final
               shot[i].active = 0;
               --currentshots;
               move(shot[i].r + 1, shot[i].c);
               addch(' ');
            }
         }
      }
   }
   pthread_mutex_unlock(&shot_mutex);
   pthread_mutex_unlock(&score_mutex);
   pthread_mutex_unlock(&pointer_mutex);
   pthread_mutex_unlock(&alien_mutex);
}

// Funcion que mueve a los aliens
void move_aliens()
{
   if (loops % settings.alien_speed == 0)
   {
      pthread_mutex_lock(&alien_mutex);
      pthread_mutex_lock(&pointer_mutex);
      // Actualizando tiempo de apairicion de cada alien del juego
      for (int i = 0; i < settings.alien_amount; i++)
      {
         // Si ya debe aparecer en pantalla, asignale una posicion de carga
         if (aliens[i].spawn_time == 0)
         {
            aliens[i].showed = current_alien;
            current_alien++;
         }
         // Si aun no debe aparecer en pantalla, disminuye su tiempo de aparicion
         if (aliens[i].spawn_time != -1)
         {
            aliens[i].spawn_time--;
         }
      }
      // Liberando paginas ocupadas por aliens muertos y asignando prioridad respectiva
      for (int i = 0; i < settings.ingame_aliens; i++)
      {
         // Verificando si existe alguna pagina ocupada por un alien obsoleto (ya muerto)
         if (ingame_aliens[i].alive == 0 && ingame_aliens[i].showed != 0 && alien_out_time[i] == -1)
         {
            // Hallando prioridad mas alta asignada a una pagina para definir la de la pagina actual
            int max = -1;
            for (int j = 0; j < settings.ingame_aliens; j++)
            {
               if (alien_out_time[j] != -1 && alien_out_time[j] > max)
               {
                  max = alien_out_time[j];
               }
            }
            // Asignando prioridad a la pagina actual y liberandola
            alien_out_time[i] = max + 1;
         }
      }
      // Ejecutando reemplazo de paginas e insertando aliens nuevos en pantalla
      for (int i = 0; i < settings.alien_amount; i++)
      {
         // Verificando si existe algun alien que deba aparecer en pantalla
         if (aliens[i].showed == current_showed_alien)
         {
            // Verificando si existe alguna pagina disponible para el alien (prioridad mas alta == valor mas bajo)
            for (int j = 0; j < settings.ingame_aliens; j++)
            {
               // Reemplazando pagina y asignando al nuevo alien
               if (alien_out_time[j] == 0)
               {
                  alien_out_time[j] = -1;
                  ingame_aliens[j].alive = aliens[i].alive;
                  ingame_aliens[j].c = aliens[i].c;
                  ingame_aliens[j].ch = aliens[i].ch;
                  ingame_aliens[j].direction = aliens[i].direction;
                  ingame_aliens[j].pc = aliens[i].pc;
                  ingame_aliens[j].pr = aliens[i].pr;
                  ingame_aliens[j].r = aliens[i].r;
                  ingame_aliens[j].showed = aliens[i].showed;
                  ingame_aliens[j].spawn_time = aliens[i].spawn_time;
                  current_showed_alien++;
               }
               // Aumentando la prioridad de las otras paginas (disminuyendo valor)
               else
               {
                  if (alien_out_time[j] > 0)
                  {
                     alien_out_time[j]--;
                  }
               }
            }
         }
      }
      // Moviendo a los aliens en pantalla a sus respectivas posiciones siguientes
      for (int i = 0; i < settings.ingame_aliens; ++i)
      {
         if (ingame_aliens[i].alive == 1)
         {
            // Borrar alien viejo
            move(ingame_aliens[i].pr, ingame_aliens[i].pc);
            addch(' ');
            // Poner alien nuevo
            move(ingame_aliens[i].r, ingame_aliens[i].c);
            addch(ingame_aliens[i].ch);
            // Actualizar posiciones viejas de alien
            ingame_aliens[i].pr = ingame_aliens[i].r;
            ingame_aliens[i].pc = ingame_aliens[i].c;

            // Verificar si deberia soltar una bomba ahora (los de tipo V no tiran bombas)
            if (ingame_aliens[i].ch != 'V')
            {
               int n_random = 1 + (rand() % 100);
               if ((settings.bombchance - n_random) >= 0 && currentbombs < settings.bombs_amount)
               {
                  for (int j = 0; j < settings.bombs_amount; ++j)
                  {
                     if (bomb[j].active == 0)
                     {
                        bomb[j].active = 1;
                        ++currentbombs;
                        bomb[j].r = ingame_aliens[i].r + 1;
                        bomb[j].c = ingame_aliens[i].c;
                        break;
                     }
                  }
               }
            }

            // Estableciendo posicion actual del alien
            if (ingame_aliens[i].direction == 'l')
               --ingame_aliens[i].c;
            else if (ingame_aliens[i].direction == 'r')
               ++ingame_aliens[i].c;
            else if (ingame_aliens[i].direction == 'd')
               ++ingame_aliens[i].r;

            // Verificando proxima posicion del alien
            if (ingame_aliens[i].ch == 'V')
            {
               if (ingame_aliens[i].direction == 'd')
               {
                  ingame_aliens[i].direction = 's';
               }
               else if (ingame_aliens[i].direction == 's')
               {
                  ingame_aliens[i].direction = 'S';
               }
               else if (ingame_aliens[i].direction == 'S')
               {
                  ingame_aliens[i].direction = 'z';
               }
               else if (ingame_aliens[i].direction == 'z')
               {
                  ingame_aliens[i].direction = 'd';
               }
            }
            else if (ingame_aliens[i].ch == 'T')
            {
               int rdm = (rand() % 3) + 1;
               if (rdm <= 1)
               {
                  if (ingame_aliens[i].pd == 's')
                  {
                     ingame_aliens[i].direction = 'l';
                  }
                  else
                  {
                     ingame_aliens[i].direction = 's';
                  }
               }
               else if (rdm <= 2)
               {
                  if (ingame_aliens[i].pd == 'l')
                  {
                     ingame_aliens[i].direction = 'r';
                  }
                  else
                  {
                     ingame_aliens[i].direction = 'l';
                  }
               }
               else if (rdm <= 3)
               {
                  if (ingame_aliens[i].pd == 'r')
                  {
                     ingame_aliens[i].direction = 's';
                  }
                  else
                  {
                     ingame_aliens[i].direction = 'r';
                  }
               }
            }
            // Definiendo accion en los bordes
            if (ingame_aliens[i].c == COLS - 2)
            {
               ++ingame_aliens[i].r;
               ingame_aliens[i].direction = 'l';
            }
            else if (ingame_aliens[i].c == 0)
            {
               ++ingame_aliens[i].r;
               ingame_aliens[i].direction = 'r';
            }
         }
      }
      pthread_mutex_unlock(&pointer_mutex);
      pthread_mutex_unlock(&alien_mutex);
   }
}

// Actualizar pantalla
void act_screen()
{
   // Refrescar pantalla
   pthread_mutex_lock(&pointer_mutex);
   move(0, 0);
   refresh();
   pthread_mutex_unlock(&pointer_mutex);
   // Esperar tantos milisegundos como se haya definido en las opciones (velocidad del juego)
   usleep(settings.overall_speed);
   // Comenzar siguiente loop (ciclo de juego)
   ++loops;
}

// Dibujar menu principal
void draw_main_menu()
{
   clear();
   move((LINES / 2) - 10, (0));
   addstr("MATCOM INVADERS");
   move((LINES / 2) - 9, (0));
   addstr("UN JUEGO HECHO POR: Laura Martir Beltrán & Luis Daniel Silva Martínez");
   move((LINES / 2) - 6, (0));
   addstr("1-Empezar partida");
   move((LINES / 2) - 5, (0));
   addstr("2-Personalizar dificutad");
   move((LINES / 2) - 4, (0));
   addstr("3-Salir del juego");
   move((LINES / 2) - 2, (0));
   addstr("Dificultad actual:");
   move((LINES / 2) - 2, (20));
   if (difficult == 0)
   {
      addstr("FACIL");
   }
   else if (difficult == 1)
   {
      addstr("NORMAL");
   }
   else if (difficult == 2)
   {
      addstr("DIFICIL");
   }
   else if (difficult == 3)
   {
      addstr("EXTREMO");
   }
   else
   {
      addstr("PERSONALIZADA");
   }
   move((LINES)-3, (0));
   addstr("PRESIONE [1,2,3] PARA SELECCIONAR UNA OPCION");
   move((LINES)-2, (0));
   addstr("PRESIONE [ESPACIO] PARA CAMBIAR LA DIFICULTAD");
   move(0, COLS - 1);
   refresh();
}

// Dibujar menu de opciones
void draw_options_menu()
{
   // Configurando biblioteca
   clear();
   echo();
   nocbreak();
   nodelay(stdscr, 0);
   // Dibujando menu
   move(0, 0);
   addstr("1. CAMBIAR LA VELOCIDAD DEL JUEGO");
   move(1, 0);
   addstr("2. CAMBIAR LA VELOCIDAD DE MOVIMIENTO DE ALIENS");
   move(2, 0);
   addstr("3. CAMBIAR LA VELOCIDAD DE DISPARO DEL JUGADOR");
   move(3, 0);
   addstr("4. CAMBIAR LA VELOCIDAD DE LAS BOMBAS");
   move(4, 0);
   addstr("5. CAMBIAR LA VELOCIDAD DE DISPARO DE ALIENS");
   move(5, 0);
   addstr("6. CAMBIAR LA CANTIDAD TOTAL DE ALIENS A ELIMINAR");
   move(6, 0);
   addstr("7. CAMBIAR LA CANTIDAD MAXIMA DE DISPAROS");
   move(7, 0);
   addstr("8. CAMBIAR LA CANTIDAD MAXIMA DE BOMBAS");
   move(8, 0);
   addstr("9. CAMBIAR LA CANTIDAD MAXIMA DE ALIENS EN PANTALLA");
   move(9, 0);
   addstr("R. REGRESAR AL MENU PRINCIPAL");
   move(10, 0);
   addstr("Q. SALIR DEL JUEGO");
   move(11, 0);
   addstr("INTRODUZCA SU OPCION: ");
   refresh();
}

//-------------------------------------------------------------IMPLEMENTANDO OTRAS FUNCIONES-------------------------------------------------------------------

// Recibir entrada del teclado
void write_input()
{
   // Recibir entrada
   input = getch();
   // Borrar posicion vieja del tanque
   pthread_mutex_lock(&pointer_mutex);
   pthread_mutex_lock(&player_mutex);
   move(tank.r, tank.c);
   addch(' ');
   pthread_mutex_unlock(&player_mutex);
   pthread_mutex_unlock(&pointer_mutex);
   //  Verificar entrada del teclado
   if (input == 'q')
   {
      pthread_mutex_lock(&win_mutex);
      win = 3;
      pthread_mutex_unlock(&win_mutex);
   }
   // Actualizar posicion del jugador
   else if (input == KEY_LEFT)
   {
      pthread_mutex_lock(&player_mutex);
      --tank.c;
      pthread_mutex_unlock(&player_mutex);
   }
   else if (input == KEY_RIGHT)
   {
      pthread_mutex_lock(&player_mutex);
      ++tank.c;
      pthread_mutex_unlock(&player_mutex);
   }
   // Efectuar disparo
   else if (input == ' ' && currentshots < settings.shots_amount)
   {
      pthread_mutex_lock(&shot_mutex);
      for (int i = 0; i < settings.shots_amount; ++i)
      {
         if (shot[i].active == 0)
         {
            // Dibujar nuevo disparo
            shot[i].active = 1;
            ++currentshots;
            --score;
            shot[i].r = LINES - 2;
            shot[i].c = tank.c;
            break;
         }
      }
      pthread_mutex_unlock(&shot_mutex);
   }
   // Abrir menu de opciones
   else if (input == 'p' || input == 'P')
   {
      pause_menu();
   }
}

// Definir configuracion normal de las variables
void init_config()
{
   // Definiendo opciones por defecto
   settings.overall_speed = 15000;
   settings.alien_speed = 15;
   settings.shots_speed = 3;
   settings.bombs_speed = 10;
   settings.bombchance = 5;
   settings.alien_amount = 20;
   settings.shots_amount = 4;
   settings.bombs_amount = 1000;
   settings.ingame_aliens = 10;
}

// Definir configuracion facil
void init_config_e()
{
   // Definiendo opciones por defecto
   settings.overall_speed = 15000;
   settings.alien_speed = 18;
   settings.shots_speed = 2;
   settings.bombs_speed = 18;
   settings.bombchance = 5;
   settings.alien_amount = 10;
   settings.shots_amount = 5;
   settings.bombs_amount = 1000;
   settings.ingame_aliens = 6;
}

// Definir configuracion dificil
void init_config_h()
{
   // Definiendo opciones por defecto
   settings.overall_speed = 15000;
   settings.alien_speed = 11;
   settings.shots_speed = 3;
   settings.bombs_speed = 10;
   settings.bombchance = 6;
   settings.alien_amount = 30;
   settings.shots_amount = 3;
   settings.bombs_amount = 1000;
   settings.ingame_aliens = 25;
}

// Definir configuracion extrema
void init_config_x()
{
   // Definiendo opciones por defecto
   settings.overall_speed = 15000;
   settings.alien_speed = 8;
   settings.shots_speed = 5;
   settings.bombs_speed = 10;
   settings.bombchance = 8;
   settings.alien_amount = 40;
   settings.shots_amount = 2;
   settings.bombs_amount = 1000;
   settings.ingame_aliens = 20;
}

// Definiendo configuracion al iniciar el juego
void starting_game()
{
   // Reestableciendo variables globales
   loops = 0;
   currentshots = 0;
   currentbombs = 0;
   currentaliens = settings.alien_amount;
   current_alien = 1;
   current_showed_alien = 1;
   score = 0;
   win = -1;

   // Definiendo opciones del jugador
   tank.r = LINES - 1;
   tank.c = COLS / 2;
   tank.ch = '^';

   // Definiendo opciones de aliens
   aliens = malloc(sizeof(t_alien) * settings.alien_amount);
   ingame_aliens = malloc(sizeof(t_alien) * settings.ingame_aliens);
   alien_out_time = malloc(sizeof(int) * settings.ingame_aliens);

   for (int i = 0; i < settings.alien_amount; i++)
   {
      aliens[i].pd = 's';
      aliens[i].showed = 0;
      aliens[i].alive = 1;
      // Definiendo posicion inicial
      int rdm = (rand() % 3) + 1;
      aliens[i].r = rdm;
      rdm = (rand() % COLS - 1) + 1;
      aliens[i].c = rdm;
      // Posiciones anteriores son 0,0
      aliens[i].pr = 0;
      aliens[i].pc = 0;
      // Definiendo tipo de alien
      rdm = (rand() % 3) + 1;
      if (rdm <= 1)
      {
         aliens[i].ch = 'M';
      }
      else if (rdm <= 2)
      {
         aliens[i].ch = 'V';
      }
      else
      {
         aliens[i].ch = 'T';
      }
      // Definiendo direccion de movimiento inicial
      rdm = (rand() % 3) + 1;
      // Aliens de tipo T
      if (aliens[i].ch == 'T')
      {
         if (rdm <= 1)
         {
            aliens[i].direction = 'r';
         }
         else if (rdm <= 2)
         {
            aliens[i].direction = 'l';
         }
         else
         {
            aliens[i].direction = 'd';
         }
      }
      // Aliens de tipo M
      else if (aliens[i].ch == 'M')
      {
         if (rdm <= 1.5)
         {
            aliens[i].direction = 'r';
         }
         else
         {
            aliens[i].direction = 'l';
         }
      }
      // Aliens de tipo V
      else if (aliens[i].ch == 'V')
      {
         aliens[i].direction = 'd';
      }
      rdm = (rand() % settings.ingame_aliens) + 1;
      aliens[i].spawn_time = rdm;
   }

   // Definiendo paginas iniciales de aliens
   for (int i = 0; i < settings.ingame_aliens; i++)
   {
      ingame_aliens[i].alive = 0;
      ingame_aliens[i].showed = 0;
      alien_out_time[i] = i;
   }

   // Definiendo opciones de disparos
   for (int i = 0; i < settings.shots_amount; ++i)
   {
      shot[i].active = 0;
      shot[i].ch = '*';
   }

   // Definiendo opciones de bombas (aliens)
   for (int i = 0; i < settings.bombs_amount; ++i)
   {
      bomb[i].active = 0;
      bomb[i].ch = 'o';
      bomb[i].loop = 0;
   }
}

//-------------------------------------------------------------------DECLARANDO HILOS--------------------------------------------------------------------------

void *game_logic_thread(void *arg)
{
   while (1)
   {

      //Mostrar puntuacion
      show_stats();
      // Mover jugador
      move_player();
      // Mover bombas
      move_bombs();
      // Mover disparos del jugador
      move_shots();
      // Mover aliens
      move_aliens();
      // Actualizar pantalla y loops
      act_screen();
      // Ejecutar entrada del teclado
      write_input();

      //Aqui no pongo mutex porque solo estoy leyendo el valor
      if (!(win == -1))
      {
         break;
      }
   }
   return NULL;
}

void *events_thread(void *arg)
{
   while (1)
   {
      // Verificar si el juego se gano o perdio:
      // Ya no quedan aliens
      pthread_mutex_lock(&alien_mutex);
      pthread_mutex_lock(&bomb_mutex);
      pthread_mutex_lock(&win_mutex);
      if (currentaliens == 0)
      {
         win = 1;
      }
      // Algun alien llego al final
      for (int i = 0; i < settings.ingame_aliens; ++i)
      {
         if (ingame_aliens[i].r == LINES - 1)
         {
            win = 2;
            pthread_mutex_unlock(&bomb_mutex);
            pthread_mutex_unlock(&alien_mutex);
            break;
         }
      }
      // Alguna bomba impacto al jugador
      for (int i = 0; i < settings.bombs_amount; ++i)
      {
         if (bomb[i].r == tank.r && bomb[i].c == tank.c)
         {
            win = 0;
            pthread_mutex_unlock(&bomb_mutex);
            pthread_mutex_unlock(&alien_mutex);
            break;
         }
      }
      pthread_mutex_unlock(&bomb_mutex);
      pthread_mutex_unlock(&alien_mutex);
      // Comprobando si termino el juego
      if (!(win == -1))
      {
         pthread_mutex_unlock(&win_mutex);
         break;
      }
      pthread_mutex_unlock(&win_mutex);
   }
   return NULL;
}

//-----------------------------------------------------------------FUNCIONES PRINCIPALES-----------------------------------------------------------------------

// Funcion que maneja el gameplay
void game()
{
   // Creando hilos
   pthread_t event_th, game_th;
   // Inicializando funciones de hilos
   pthread_create(&event_th, NULL, events_thread, NULL);
   pthread_create(&game_th, NULL, game_logic_thread, NULL);
   // Ciclo de juego
   while (1)
   {
      pthread_mutex_lock(&win_mutex);
      if (!(win == -1))
      {
         pthread_mutex_unlock(&win_mutex);
         break;
      }
      pthread_mutex_unlock(&win_mutex);
      sleep(1);
   }
   // Esperando a que terminen los hilos
   pthread_join(event_th, NULL);
   pthread_join(game_th, NULL);

   // Liberando memoria de los arrays reservados
   free(aliens);
   free(ingame_aliens);
   free(alien_out_time);
   // Terminando partida
   gameover(win);
}

// Funcion principal que maneja la entrada del usuario, la pantalla de juego y condiciones de victoria/derrota
int main(int argc, char const *argv[])
{
   // Inicializar biblioteca curses (para mejorar el uso de la pantalla)
   initscr();
   clear();  // limpiar consola
   noecho(); // no escribir caracter presionado
   cbreak();
   nodelay(stdscr, 1);
   keypad(stdscr, 1);
   srand(time(NULL));

   // Establecer configuracion inicial
   init_config();
   // Mostrando menu principal
   while (1)
   {
      main_menu();
   }
}