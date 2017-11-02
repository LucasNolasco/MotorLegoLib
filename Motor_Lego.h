#ifndef MOTOR_LEGO_H
#define MOTOR_LEGO_H
#include <Arduino.h>

class Motor_Lego
{
public:
    Motor_Lego(int _en, int _m1, int _m2, int _enc1, int _enc2, int _raio);
    void moverDistancia(int i, int j);
    void moverVelocidade(int l);
    void girarPID(int _graus);
    void girar(int _graus, int _vel);
    void girarPara(int _graus);
    int posicao();
    int getKa();
    int getKb();
    void giro(int step_novo, int sentido);
    void setPID(int _kp, int _ki, int _kd);

private:
    int graus, step, ultimo_step, penult_step;  
    int interrupt[6] = {2, 3, 21, 20, 19, 18};
    int en, m1, m2, enc1, enc2, ka, kb;
    int raio, pose_init, var1, var2;
    int ki, kp, kd, erro;
    int tol = 10;
    static Motor_Lego** motores;
    static bool init;

    friend void contadorGiros();
};

class Eixo{
  public:
     Eixo(int a2, int b2, int c2, int d2, int e2, int f2, int g2, int h2);
     void moverDistancia(int i, int j);
     void moverVelocidade(int l);
	void girarPara(int o, int p);
     int posicao();
private:
     static void giro0();
     static void giro1();
     static void giro2();
     static void giro3();
     static void giro4();
     static void giro5();
     static int grau0, grau1, grau2, grau3, grau4, grau5;
     int graus;
     int interrupt[6] = {2, 3, 21, 20, 19, 18};
     int en_e, me1, me2, en_d, md1, md2, enc, k;
     int raio, pose_init, var1, var2;
     int tol = 10;
};

#endif
