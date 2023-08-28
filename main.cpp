#include <SOIL/SOIL.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//Escopo de Funções

void iniciaAliens(void);
void verificaAlienChegouFim(void);
void deslocaAliens(GLfloat eixoX, GLfloat eixoY);
void movimentaAliens(int value);
void darTiroAliens(void);
void desenhaNave(void);
void movimentaTiro(void);
void movimentaObjetos(int value);
void desenhaTiro(void);
void desenhaTiroAlien(void);
void desenhaTexto(char string[], int x, int y, int fonte);
void desenhaPlacar(void);
void desenhaVidas(void);
void desenhaFundo(void);
void desenhaGameOver(void);
void desenhaVenceuJogo(void);
void desenhaMenu(void);
void desenhaMinhaCena(void);
void inicializa(void);
void reiniciaJogo(void);
void tecla(unsigned char key, int x, int y);
void teclaEspecial(GLint key, int x, int y);

GLint jogoPausado = 0; // (0) jogo despausado e (1) jogo pausado
GLint gameOver = 0; // (1) fim de jogo e (0) jogo em andamento
GLint menu = 1; //jogo comeca no menu
GLint placar = 0;
GLint vidas = 3;
GLint venceuJogo = 0;

//variavel para mover a nave horizontalmente
GLfloat posNaveX = 50; //comecar a nave no centro da tela
GLfloat posTiroY = 100; //tiro fora da tela (invisivel inicialmente)
GLfloat posTiroX = -10; //tiro fora da tela (invisivel inicialmente)

GLint sentidoAliensX = 1; //sentido do movimento no eixo x: esquerda (0) e direita (1). Começar indo para a direita

GLuint idTexturaNave;

GLuint idTexturaFundo;

GLuint idTexturaAlien;

GLuint idTexturaVida;


GLuint carregaTextura(const char* arquivo) {
    GLuint idTextura = SOIL_load_OGL_texture(
                           arquivo,
                           SOIL_LOAD_AUTO,
                           SOIL_CREATE_NEW_ID,
                           SOIL_FLAG_INVERT_Y
                       );

    if (idTextura == 0) {
        printf("Erro do SOIL: '%s'\n", SOIL_last_result());
    }

    return idTextura;
}


struct alien {
  GLfloat posX;
  GLfloat posY;
  GLint estaVivo;
};

struct tiroAlien {
  GLfloat posX;
  GLfloat posY;
};

struct alien aliens[3][7]; // matriz de aliens 3x7
struct tiroAlien tiroalien;

void iniciaAliens() {
  int i, j;
  for(i=0; i<3; i++) {
    for(j=0; j<7; j++) {
      aliens[i][j].posX = j*10 + 10;
      aliens[i][j].posY = 100-(i*10 + 10);
      aliens[i][j].estaVivo = 1;
    }
  }

  tiroalien.posX = 0;
  tiroalien.posY = -10; // comecar com o tiro do alien fora da tela
}

void desenhaAliens() { //Cada alien tem hit box de um quadrado 3x3
    int i, j;
    for(i=0; i<3; i++) {
      for(j=0; j<7; j++) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, idTexturaAlien);
        glBegin(GL_POLYGON);
            if(aliens[i][j].estaVivo) { //so desenha o alien se ele estiver vivo

              glTexCoord2f(0, 0);
              glVertex3f(aliens[i][j].posX-3, aliens[i][j].posY-3, 0);
              glTexCoord2f(0, 1);
              glVertex3f(aliens[i][j].posX-3, aliens[i][j].posY+3, 0);
              glTexCoord2f(1, 1);
              glVertex3f(aliens[i][j].posX+3, aliens[i][j].posY+3, 0);
              glTexCoord2f(1, 0);
              glVertex3f(aliens[i][j].posX+3, aliens[i][j].posY-3, 0);
            }

        glEnd();
        glDisable(GL_TEXTURE_2D);
      }
    }
}

void verificaAlienChegouFim() {
  //testa pra ver se o alien chegou no final do eixo y
  int i, j;
  for(i=0; i<3; i++) {
    for(j=0; j<7; j++) {
      if(aliens[i][j].estaVivo) {
        if(aliens[i][j].posY < 12) {
          //alien ganhou
          gameOver = 1;
        }
      }
    }
  }
}

void deslocaAliens(GLfloat eixoX, GLfloat eixoY) {
  int i, j;
  //faz o movimento dos aliens na horizontal
  for(i=0; i<3; i++) {
    for(j=0; j<7; j++) {
      aliens[i][j].posX += eixoX;
      aliens[i][j].posY += eixoY;
    }
  }
}

void movimentaAliens() {
  int i, j;

  int menorX = 100;
  int maiorX = 0;
  for(i=0; i<3; i++) {
    for(j=0; j<7; j++) {
      if(aliens[i][j].estaVivo) {
        if(aliens[i][j].posX > maiorX) {//Identifica quando os aliens chegam ao canto direito
          maiorX = aliens[i][j].posX;
        }
        if(aliens[i][j].posX < menorX) {//Identifica quando os aliens chegam ao canto esquerdo
          menorX = aliens[i][j].posX;
        }
      }
    }
  }

  //identifica o sentido x dos aliens (direita ou esquerda)
  if(maiorX >= 97) {
    sentidoAliensX = 0; //inverte o sentido (para de ir para direita e comeca ir pra esquerda)
    deslocaAliens(0, -3); //desloca os aliens para baixo em 3 pixels no eixo y
    verificaAlienChegouFim();
  }
  if(menorX <= 3) {
    sentidoAliensX = 1;//inverte o sentido (para de ir para esquerda e comeca ir pra direita)
    deslocaAliens(0, -3); //desloca os aliens para baixo em -3 pixels no eixo y
    verificaAlienChegouFim();
  }

  //faz o movimento dos aliens na horizontal
  if(sentidoAliensX == 1) {
    deslocaAliens(0.5, 0); //desloca os aliens na horizontal em 0.5 pixels
  } else {
    deslocaAliens(-0.5, 0); //desloca os aliens na horizontal em -0.5 pixels
  }

}



void darTiroAliens() {
  if(tiroalien.posY <= -10) { //só atira novamente se o ultimo tiro estiver fora da tela)
    int deuTiro = 0;
    int xAleatorio, yAleatorio;
    int index = 0;
    //escolhe aleatoriamente um alien para atirar, 50 tentativas para encontrar um alien vivo
    while(deuTiro == 0 && index <= 50) {
      xAleatorio = rand() % 3; //x tem ate 3 linhas
      yAleatorio = rand() % 7; //y tem ate 3 colunas
      if(aliens[xAleatorio][yAleatorio].estaVivo) {
        tiroalien.posX = aliens[xAleatorio][yAleatorio].posX;
        tiroalien.posY = aliens[xAleatorio][yAleatorio].posY -3;
        deuTiro = 1;
      }
      index++;
    }
  } else {
    tiroalien.posY--;

    // verificar se colidiu o tiro do alien com a nave
    if(tiroalien.posX >= posNaveX-3 && tiroalien.posX <= posNaveX+3) {
      if(tiroalien.posY >= 2 && tiroalien.posY <= 10) {
        //se acertou, manda o tiro para fora da tela, para nao dar game over com 3 vidas
        tiroalien.posY = -10;
        vidas--;
        if(vidas == 0) {
          gameOver = 1;
        }
      }
    }
  }
}

void desenhaNave() {//Nave tem hitbox de um quadrado 4x4

    // Habilita o uso de texturas
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, idTexturaNave);

    // desenha um POLYGON por seus vertices
    glBegin(GL_POLYGON);

        glTexCoord2f(0, 0);
        glVertex3f(posNaveX-4, 2, 0);

        glTexCoord2f(0, 1);
        glVertex3f(posNaveX-4, 11, 0);

        glTexCoord2f(1, 1);
        glVertex3f(posNaveX+4, 11, 0);

        glTexCoord2f(1, 0);
        glVertex3f(posNaveX+4, 2, 0);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

void movimentaTiro() {
  int i, j;
  posTiroY += 2;

  //verifica se o tiro encostou em algum aliens. Verifica todos os aliens
  if(posTiroY >= 0 && posTiroY <= 100) { //somente verificar se encostou se o tiro estiver na tela.
    for(i=0; i<3; i++) {
      for(j=0; j<7; j++) {
        if(posTiroY >= aliens[i][j].posY-3 && posTiroY <= aliens[i][j].posY+3) {
          if(posTiroX >= aliens[i][j].posX-3 && posTiroX <= aliens[i][j].posX+3) {
            //verifica se esse alien esta vivo. Se estiver morto, nao pode morrer de novo
            if(aliens[i][j].estaVivo) {
              //se cair aqui, significa que o tiro encostou no alien
              aliens[i][j].estaVivo = 0;
              posTiroX = -10;
              posTiroY = 100;
              placar++;
              if(placar == 21) {
                tiroalien.posY = -10; //quando vencer o jogo, sumir com o tiro da nave
                venceuJogo = 1;
              }
            }
          }
        }
      }
    }
  }
}

void movimentaObjetos(int value) {
  if(!jogoPausado && !gameOver && !menu) { //nao movimenta os objetos se o jogo estiver pausado, ou gameover ou se estiver na splash
    movimentaTiro();
    movimentaAliens();
    darTiroAliens();
  }
  glutPostRedisplay();
  glutTimerFunc(33, movimentaObjetos, 1);
}

void desenhaTiro() {
    glBegin(GL_POLYGON);
        glColor3f(1, 1, 1); // cor do tiro (amarelo)
        glVertex3f(posTiroX-0.5, posTiroY-2, 0);
        glVertex3f(posTiroX-0.5, posTiroY+2, 0);
        glVertex3f(posTiroX+0.5, posTiroY+2, 0);
        glVertex3f(posTiroX+0.5, posTiroY-2, 0);
    glEnd();
}

void desenhaTiroAlien() {
    glBegin(GL_POLYGON);
        glColor3f(1, 1, 1); // cor do tiro do alien (verde)
        glVertex3f(tiroalien.posX-0.5, tiroalien.posY-2, 0);
        glVertex3f(tiroalien.posX-0.5, tiroalien.posY+2, 0);
        glVertex3f(tiroalien.posX+0.5, tiroalien.posY+2, 0);
        glVertex3f(tiroalien.posX+0.5, tiroalien.posY-2, 0);
    glEnd();
}

void desenhaTexto(char string[], int x, int y, int fonte) {
  glColor3f(1,1,1); //texto branco
  glRasterPos2f(x, y);
  int i;
  for (i = 0; i < (int)strlen(string); i++) {
    if(fonte == 1) {
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, string[i]);
    } else if (fonte == 2) {
      glutBitmapCharacter(GLUT_BITMAP_9_BY_15, string[i]);
    }
  }
}

void desenhaPlacar()
{
  char string[20];
  snprintf(string, 20, "Aliens Abatidos: %d", placar);
  desenhaTexto(string, 66, 95, 1);
}


void desenhaVidas()
{
  desenhaTexto("Vidas:", 5, 95, 1);
  int i;
  for(i=0;i<vidas;i++) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, idTexturaVida);

    // desenha um POLYGON por seus vertices
    glBegin(GL_POLYGON);

        glTexCoord2f(0, 0);
        glVertex3f(18+(i*5), 94, 0);

        glTexCoord2f(0, 1);
        glVertex3f(18+(i*5), 98, 0);

        glTexCoord2f(1, 1);
        glVertex3f(22+(i*5), 98, 0);

        glTexCoord2f(1, 0);
        glVertex3f(22+(i*5), 94, 0);
    glEnd();
    glDisable(GL_TEXTURE_2D);
  }
}


void desenhaFundo() {
  // Habilita o uso de texturas
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, idTexturaFundo);

    // desenha um POLYGON por seus vertices
    glBegin(GL_POLYGON);
        glTexCoord2f(0, 0);
        glVertex3f(0, 0, 0);

        glTexCoord2f(0, 1);
        glVertex3f(0, 100, 0);

        glTexCoord2f(1, 1);
        glVertex3f(100, 100, 0);

        glTexCoord2f(1, 0);
        glVertex3f(100, 0, 0);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

void desenhaGameOver() {
    if(gameOver) {
      // desenha um POLYGON por seus vertices
      glBegin(GL_POLYGON);
          glColor3f(0, 0, 0);
          glVertex3f(0, 0, 0);

          glVertex3f(0, 100, 0);

          glVertex3f(100, 100, 0);

          glVertex3f(100, 0, 0);
      glEnd();
      glDisable(GL_TEXTURE_2D);
      desenhaTexto("GAME OVER. [ R ] para reiniciar", 25, 50, 1);
    }
}

void desenhaVenceuJogo() {
  if(venceuJogo) {
    desenhaTexto("MISSION COMPLETE!", 33, 70, 1);
    desenhaTexto("[ R ] para jogar novamente", 30, 64, 1);
  }
}

void desenhaMenu() {
 if(menu) {
    // desenha um POLYGON por seus vertices
    glBegin(GL_POLYGON);
        glColor3f(0, 0, 0);
        glVertex3f(0, 0, 0);

        glVertex3f(0, 100, 0);

        glVertex3f(100, 100, 0);

        glVertex3f(100, 0, 0);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    desenhaTexto("WE ARE THE GALAXIANS", 28, 75, 1);
    desenhaTexto("MISSION: DESTROY ALL THE ALIENS", 16, 70, 1);
    desenhaTexto("Controles:", 10, 35, 2);
    desenhaTexto("[ESPACO]: Iniciar o jogo // Atirar", 10, 30, 2);
    desenhaTexto("[<-] e [->]: Controla a Nave", 10, 25, 2);
    desenhaTexto("[P]: Pausar", 10, 20, 2);
    desenhaTexto("[R]: Reiniciar", 10, 15, 2);
    desenhaTexto("[ESC]: Quit", 10, 10, 2);
  }
}
// callback de desenho (nave)
void desenhaMinhaCena() {
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 100, 0, 100, -1, 1); //tela divida em 100 "pedacos" no eixo x e y

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    desenhaFundo();
    desenhaAliens();
    desenhaTiro();
    desenhaTiroAlien();
    desenhaNave();

    if(jogoPausado) {
      desenhaTexto("Jogo Pausado.[ P ] para retornar.", 25, 50, 1);
    }

    desenhaPlacar();
    desenhaVidas();
    desenhaVenceuJogo();

    desenhaMenu();
    desenhaGameOver();

    glFlush();
}

void inicializa() {
    // cor do fundo (Background) preto
    glClearColor(0, 0, 0, 0);

    // habilita mesclagem de cores, para termos suporte a texturas
    // com transparência
    glEnable(GL_BLEND );
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    idTexturaNave = carregaTextura("nave.png");
    idTexturaFundo = carregaTextura("fundo.png");
    idTexturaAlien = carregaTextura("alien1.png");
    idTexturaVida = carregaTextura("vidas.png");

}

void reiniciaJogo() {
  iniciaAliens();
  jogoPausado = 0;
  posNaveX = 50; // reinicia a posicao da nave
  posTiroY = 100; // reinicia a posicao do tiro
  posTiroX = -10; // reinicia a posicao do tiro
  sentidoAliensX = 1;
  gameOver = 0;
  vidas = 3;
  placar = 0;
  venceuJogo = 0;
}


void tecla(unsigned char key, int x, int y) {
    // verifica qual tecla foi pressionada
    switch(key) {
    case 27:      // "ESC"
        exit(0);  // encerra a aplicacao
        break;

    case 32:      // barra de espaco
        if(menu) { // se estiver na tela de splash, entao inicia o jogo, senao, faz a logica do tiro
          menu = 0;
          reiniciaJogo();
        }
        else if(!jogoPausado){ //nao dar tiro com o jogo pausado
          // so pode dar um tiro de cada vez. Portanto, verifica se a posicao do tiro e maior que 100. Senao, precisa esperar o tiro para apertar de novo
          if(posTiroY >= 100) {
            posTiroY = 11; //posicao 11 quando da o tiro, para parecer que esta saindo da nave e nao no inicio da tela
            posTiroX = posNaveX;
          }
        }
        break;
    case 'p': //pausar jogo
        jogoPausado = !jogoPausado;
        break;
    case 'r': //reiniciar jogo
        reiniciaJogo();
        menu = 1;
        break;
    default:
        break;
    }
}

void teclaEspecial(GLint key, int x, int y) {
    int i;
    // verifica qual tecla foi pressionada
    switch(key) {
      case GLUT_KEY_LEFT: //seta para esquerda
        if(!jogoPausado && posNaveX >=4) { // nao deixar a nave sair da tela e nao movimentar se estiver pausado
          posNaveX -= 4;
        }
        break;

      case GLUT_KEY_RIGHT: //seta para direita
        if(!jogoPausado && posNaveX <= 96) { // nao deixar a nave sair da tela
          posNaveX += 4;
        }
        break;

      default:
        break;
    }
    desenhaMinhaCena();
}

// função principal
int main(int argc, char** argv) {
   glutInit(&argc, argv);

   glutInitContextVersion(1, 1);
   glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

   glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
   glutInitWindowSize(500, 500);
   glutInitWindowPosition(100, 100);

   glutCreateWindow("Galaxian");

   //inicializa a matriz 3x7 de aliens
   iniciaAliens();

   // registra callbacks para alguns eventos
   glutDisplayFunc(desenhaMinhaCena);
   glutKeyboardFunc(tecla);
   glutSpecialFunc(teclaEspecial);
   glutTimerFunc(33, movimentaObjetos, 1); // 1000/33 = 30 frames por segundo

   inicializa();

   glutMainLoop();
   return 0;
}
