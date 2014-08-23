#include "Particle.h"
#include "cinder/Rand.h"
#include "cinder/dx/dx.h"
#include "cinder/Color.h"
#include "cinder/Perlin.h"

using namespace ci;

extern void renderImage( vec3 _loc, float _diam, Color _col, float _alpha );
extern float floorLevel;
extern bool ALLOWFLOOR, ALLOWGRAVITY, ALLOWPERLIN;
extern vec3 gravity;
extern int counter;

Perlin sPerlin(2);

Particle::Particle( vec3 _loc, vec3 _vel )
{
	reset( _loc, _vel);
	ISDEAD = true;

}

void Particle::reset( vec3 _loc, vec3 _vel )
{
	radius      = Rand::randFloat( 10, 40 );
	len         = (int)radius;

	startLoc = _loc + Rand::randvec3() * Rand::randFloat( 5.0f );
	vel = _vel * 0.5f + Rand::randvec3() * Rand::randFloat( 10.0f );

	perlin = vec3::zero();

	loc.clear();
	for( int i = 0; i < len; i++ ) {
		loc.push_back( startLoc );
	}

	age         = 0;
	agePer		= 1.0f;
	bounceAge   = 2;
	lifeSpan    = (int)( radius );

	ISBOUNCING	= false;
	ISDEAD		= false;

}

void Particle::update()
{
	if( ALLOWPERLIN )
		findPerlin();
	  
	findVelocity();
	setPosition();
	setAge();
}

void Particle::findPerlin()
{
	vec3 noise = sPerlin.dfBm( loc[0] * 0.01f + vec3( 0, 0, counter / 100.0f ) );
	perlin = noise.normalized() * 0.5f;
}

void Particle::findVelocity()
{
	if( ALLOWGRAVITY )
		vel += gravity;
	  
	if( ALLOWPERLIN )
		vel += perlin;

	if( ALLOWFLOOR ) {
		if( loc[0].y + vel.y > floorLevel ){
			ISBOUNCING = true;
		}
		else {
			ISBOUNCING = false;
		}
	}

	if( ISBOUNCING ) {
		vel *= 0.75f;
		vel.y *= -0.5f;
	}
}

void Particle::setPosition()
{
	for( int i = len - 1; i > 0; i-- ) {
		loc[i] = loc[i-1];
	}
	loc[0] += vel;
}

void Particle::render()
{
	Color c = Color( agePer, agePer * 0.75f, 1.0f - agePer );
	renderImage( loc[0], radius * agePer, c, 1.0f );
}

void Particle::renderTrails()
{

	dx::color( agePer, agePer * 0.25f, 1.0f - agePer, agePer * .5f );
			dx::vertex( loc[0]  + vec3(-2.0, 0.0, 0.0 ));
			dx::vertex( loc[0]  + vec3(2.0, 0.0, 0.0 ));
			dx::vertex( loc[1] + vec3(2.0, 0.0, 0.0 ));
			dx::vertex( loc[1] + vec3(-2.0, 0.0, 0.0));


			dx::vertex( loc[1] + vec3(2.0, 0.0, 0.0 ));
			dx::vertex( loc[1] + vec3(-2.0, 0.0, 0.0));
			dx::vertex( loc[2] + vec3(2.0, 0.0, 0.0 ));
			dx::vertex( loc[2] + vec3(-2.0, 0.0, 0.0));

			dx::vertex( loc[2] + vec3(2.0, 0.0, 0.0 ));
			dx::vertex( loc[2] + vec3(-2.0, 0.0, 0.0));
			dx::vertex( loc[3] + vec3(2.0, 0.0, 0.0 ));
			dx::vertex( loc[3] + vec3(-2.0, 0.0, 0.0));


			dx::vertex( loc[3] + vec3(2.0, 0.0, 0.0 ));
			dx::vertex( loc[3] + vec3(-2.0, 0.0, 0.0));
			dx::vertex( loc[4] + vec3(2.0, 0.0, 0.0 ));
			dx::vertex( loc[4] + vec3(-2.0, 0.0, 0.0));


	//for( int i = 0; i < len - 2; i++ ) {

	//	float per = 1.0f - i / (float)(len-1);
	//	vec3 perp0 = loc[i] - loc[i+1];
	//	vec3 perp1 = perp0.cross( vec3::yAxis() );
	//	vec3 perp2 = perp0.cross( perp1 );
	//		  perp1 = perp0.cross( perp2 ).normalized();
	//		  
	//	vec3 off = perp1 * ( radius * agePer * per * 0.1f );
	//	dx::color( per, per * 0.25f, 1.0f - per, per * 0.9f );

	//	dx::vertex( loc[i] - off );
	//	dx::vertex( loc[i] + off );
	//	dx::vertex( loc[i] + perp0 );
	//	dx::vertex( loc[i] + perp1 );

	//}

}

void Particle::setAge()
{
	if( ALLOWFLOOR ){
		if( ISBOUNCING ){
			age += bounceAge;
			bounceAge += 1.0f;
		}
		else {
			age += 0.25f;
		}
	}
	else {
		age += 1.0f;
	}

	if( age > lifeSpan ) {
		ISDEAD = true;
	}
	else {
		agePer = 1.0f - age / (float)lifeSpan;
	}
}