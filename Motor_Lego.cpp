﻿#include "Motor_Lego.h"

int Eixo::grau0;
int Eixo::grau1;
int Eixo::grau2;
int Eixo::grau3;
int Eixo::grau4;
int Eixo::grau5;

Motor_Lego** Motor_Lego::motores = new Motor_Lego*[6];
bool Motor_Lego::init = false;

void contadorGiros()
{
	int seq_pos[] = {3, 2, 0, 1}; // sequencia esperada para um giro 'positivo'
	int seq_neg[] = {3, 1, 0, 2}; // sequencia esperada para um giro 'negativo'
	int step, j; // variavel para salvar o passo atual do motor

	for(int i = 0; i < 6; i++) // loop para verificar todos os motores do vetor
	{
		if(Motor_Lego::motores[i] != NULL) // verifica se existe um vetor naquela posicao
		{
			step = digitalRead(Motor_Lego::motores[i]->enc2) * 2 + digitalRead(Motor_Lego::motores[i]->enc1); // confere o passo atual do motor
			for(j = 0; seq_neg[j] != Motor_Lego::motores[i]->ultimo_step; j++); // encontra o penultimo passo na lista  de sequencias
			if(seq_neg[(j + 1) % 4]  == Motor_Lego::motores[i]->step && seq_neg[(j + 2) % 4] == step) // verifica se a sequencia de passos executados bate com o esperado
				Motor_Lego::motores[i]->giro(step, -1); // registra o giro

			else // caso não coincida é verificado o giro no outro sentido
			{
				for(j = 0; seq_pos[j] != Motor_Lego::motores[i]->ultimo_step; j++); // encontra a posição do penultimo passo na sequencia

				if(seq_pos[(j + 1) % 4]  == Motor_Lego::motores[i]->step && seq_pos[(j + 2) % 4] == step) // verifica se a sequencia bate com o esperado
					Motor_Lego::motores[i]->giro(step, 1); // registra o giro
				else
				{
					Motor_Lego::motores[i]->giro(step, 0); 
				}
			}
		}
	}
}

Motor_Lego::Motor_Lego(int _en, int _m1, int _m2, int _enc1, int _enc2, int _raio)
    :en(_en), m1(_m1), m2(_m2), enc1(_enc1), enc2(_enc2), raio(_raio)
    {
	ultimo_step = 3;
	step = 2;

      	for(ka = 0; enc1 != interrupt[ka]; ka++);
      	for(kb = 0; enc2 != interrupt[kb]; kb++);
	    
	pinMode(ka, INPUT);
	pinMode(kb, INPUT);
 
 	attachInterrupt(ka, contadorGiros, CHANGE);
 	attachInterrupt(kb, contadorGiros, CHANGE);
	    
	if(!init)
	    for(int i = 0; i < 6; i++)
		Motor_Lego::motores[i] = NULL;
	    
	init = true;

 	for(int i = 0; i < 6; i++)
 	{
 	    if(Motor_Lego::motores[i] == NULL)
 	    {
 	    	Motor_Lego::motores[i] = this;
 	    	break;
 	    }
 	}

      	pinMode(en, OUTPUT);
      	pinMode(m1, OUTPUT);
      	pinMode(m2, OUTPUT);
    }

void Motor_Lego::setPID(int _kp, int _ki, int _kd)
{
	kp = _kp;
	ki = _ki;
	kd = _kd;
}

void Motor_Lego::moverDistancia(int i, int j){
      pose_init = posicao();
      while(true){
	    graus = posicao();
	    if(((graus - pose_init) * raio) < i){
         		moverVelocidade(j);
	    }
	    else if(((graus - pose_init) * raio) > i){
			moverVelocidade(-1 * j);
	    }
	    else{
			moverVelocidade(0);
			break;
	    }
      }
    }

void Motor_Lego::moverVelocidade(int l){
      if(l > 0){
        l = map(l, 0, 100, 0, 255);
        analogWrite(en, l);
        digitalWrite(m1, HIGH);
        digitalWrite(m2, LOW);
      }
      else if (l < 0){
        l = map(abs(l), 0, 100, 0, 255);
        analogWrite(en, l);
        digitalWrite(m1, LOW);
        digitalWrite(m2, HIGH);
      }
	 else{
	   analogWrite(en, 255);
	   digitalWrite(m1, HIGH);
	   digitalWrite(m2, HIGH);
	 }
    }

void Motor_Lego::girarPID(int _graus)
{
	int P, I, D, erro, last_pos, tempo, velocidade, objetivo;

	objetivo = posicao() + _graus;
	last_pos = posicao();
	tempo = millis();
	erro = objetivo - posicao();
	I = 0;

	while(abs(erro) > tol)
	{
		P = kp * erro;
		I += ki * (posicao() - last_pos) * (millis() - tempo) * 0.001;
		D =  kd * (posicao() - last_pos) / ((millis() - tempo) * 0.001);

		last_pos = posicao();
		tempo = millis();

		velocidade = P + I + D;

		if(velocidade > 100)
			velocidade = 100;

		else if(velocidade < -100)
			velocidade = - 100;

		moverVelocidade(velocidade);

		if(abs(posicao() - last_pos) > 350)
		{
			if(objetivo >= 360)
				objetivo %= 360;

			else if(objetivo < 0)
				objetivo += 360;
		}

		erro = objetivo - posicao();
	}
}

void Motor_Lego::girar(int _graus, int _vel)
{
	int erro, velocidade, objetivo;

	objetivo = posicao() + _graus;

	if(objetivo >= 360)
		objetivo %= 360;

	else if(objetivo < 0)
		objetivo += 360;

	erro = objetivo - posicao();

	if(_graus > 0)
		velocidade = _vel;

	else if(_graus < 0)
		velocidade = - _vel;

	moverVelocidade(velocidade);

	while(abs(erro) > tol)
		erro = objetivo - posicao();

	moverVelocidade(0);
}
 
void Motor_Lego::girarPara(int _graus){
	girarPID(_graus - posicao());
}

void Motor_Lego::giro(int step_novo, int sentido){
	penult_step = ultimo_step;
	ultimo_step = step;
	step = step_novo;

	if(sentido > 0)
	{
		graus++;
		if(graus >= 360)
			graus = graus % 360;
	}
	else if(sentido < 0)
	{
		graus--;
		if(graus < 0)
			graus = graus + 360;
	}
}

int Motor_Lego::posicao(){
  return this->graus;
}

int Motor_Lego::getKa(){
  return this->ka;
}

int Motor_Lego::getKb(){
  return this->kb;
}


Eixo::Eixo(int a2, int b2, int c2, int d2, int e2, int f2, int g2, int h2)
:en_e(a2), me1(b2), me2(c2), en_d(d2), md1(e2), md2(f2), enc(g2), raio(h2)
{
	k = 0;
     while(true)
     {
       if(enc == interrupt[k])
       {
		switch(k){
		case 0:	
	    		attachInterrupt(k, Eixo::giro0, RISING);
        	 	break;
		case 1:
	    		attachInterrupt(k, Eixo::giro1, RISING);
        	 	break;
		case 2:
	    		attachInterrupt(k, Eixo::giro2, RISING);
        	 	break;
		case 3:
	    		attachInterrupt(k, Eixo::giro3, RISING);
        	 	break;
		case 4:
	    		attachInterrupt(k, Eixo::giro4, RISING);
        	 	break;
		case 5:
	    		attachInterrupt(k, Eixo::giro5, RISING);
        	 	break;
		}
         	break;
        }
        k++;
      }
      pinMode(en_e, OUTPUT);
      pinMode(me1, OUTPUT);
      pinMode(me2, OUTPUT);
	 pinMode(en_d, OUTPUT);
      pinMode(md1, OUTPUT);
      pinMode(md2, OUTPUT);
}

void Eixo::moverVelocidade(int l){
      if(l >= 0){
        l = map(l, 0, 100, 0, 255);
        analogWrite(en_e, l);
        digitalWrite(me1, HIGH);
        digitalWrite(me2, LOW);
	   analogWrite(en_d, l);
        digitalWrite(md1, HIGH);
        digitalWrite(md2, LOW);
      }
      else{
        l = map(abs(l), 0, 100, 0, 255);
        analogWrite(en_e, l);
        digitalWrite(me1, LOW);
        digitalWrite(me2, HIGH);
	   analogWrite(en_d, l);
        digitalWrite(md1, LOW);
        digitalWrite(md2, HIGH);
      }
    }

void Eixo::moverDistancia(int i, int j){
	 pose_init = posicao();
      while(true){
	    if(((posicao() - pose_init) * raio) <= i){
         		moverVelocidade(j);
	    }
	    else if(((posicao() - pose_init) * raio) > i){
			moverVelocidade(-1 * j);
	    }
	    else{
			break;
	    }
      }
}

void Eixo::girarPara(int o, int p){
	while(true){
		o = o % 360;
		graus = posicao();	
		var1 = ((o + tol) % 360);
		var2 = (((o - tol) + 360) % 360);

		if(var1 > var2){
			if(graus > var1){
				moverVelocidade(p);
			}
			else if(graus < var2){
				moverVelocidade(p);
			}
			else{
				moverVelocidade(0);
				break;
			}
		}
		else{
			if(graus < var1){
				moverVelocidade(p);
			}
			else if(graus < var2){
				moverVelocidade(p);
			}
			else{
				moverVelocidade(0);
				break;
			}
		}
	}
}

int Eixo::posicao(){
  switch(k){
	case 0:
		return grau0;
		break;
	case 1:
		return grau1;
		break;
	case 2:
		return grau2;
		break;
	case 3:
		return grau3;
		break;
	case 4:
		return grau4;
		break;
	case 5:
		return grau5;
		break;
	}
}

void Eixo::giro0(){
  grau0++;
  grau0 = grau0 % 360;
}

void Eixo::giro1(){
  grau1++;
  grau1 = grau1 % 360;
}

void Eixo::giro2(){
  grau2++;
  grau2 = grau2 % 360;
}

void Eixo::giro3(){
  grau3++;
  grau3 = grau3 % 360;
}

void Eixo::giro4(){
  grau4++;
  grau4 = grau4 % 360;
}

void Eixo::giro5(){
  grau5++;
  grau5 = grau5 % 360;
}
