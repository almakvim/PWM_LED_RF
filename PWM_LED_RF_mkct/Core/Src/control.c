
#include "string.h"
#include "mk_conf_tree.h"
#include "setup.h"
#include "system.h"
#include "USART.h"
#include "control.h"

VAR_PAR dev_var;
u32 mTimeout_cntrl = 0;
u32 mTimeout_shift = 0;
//============================================================================
uint8_t Control_prop( uint8_t par, uint8_t * name, uint8_t * prop, uint8_t * nbyte )
{
	char * str;
    uint8_t size = 0;
	if( name ) { switch( par )
        {
		case CONTROL_LEVEL      :	str ="Level (0..100)"; break;
		case CONTROL_PWM        :	str ="PWM code (0..65535)"; break;
		case CONTROL_STAT       :	str ="Status"; break;
        default: return 0;
        }
        while( *str ) { *name++ = *str++; size++; } *name = 0; 
    }
	if( prop ) switch( par )
        {
		case CONTROL_STAT       :	*prop = HEX; break;
		case CONTROL_PWM        :	*prop = UINT|RO; break;
		case CONTROL_LEVEL      :	*prop = UINT; break;
        default: return 0;
        }
	if( nbyte ) switch( par )
        {
		case CONTROL_STAT       :	
		case CONTROL_PWM        :	*nbyte = 2; break;
		case CONTROL_LEVEL      :	*nbyte = 1; break;
        default: return 0;
        }
    return size;
}
//============================================================================
void Control_get(uint8_t par, void * value)
{
    switch( par )
    {
        case CONTROL_LEVEL     	:	*(u16*)value = dev_var.level; break;
        case CONTROL_PWM     	:	*(u16*)value = dev_var.pwm; break;
        case CONTROL_STAT     	:	*(u16*)value = dev_var.state; break;
        default:;
    }
}
//============================================================================
void Control_set(uint8_t par, void * value)
{
	u8 i = *(u8*)value;
    switch( par )
    {
		case CONTROL_LEVEL	  :	if(i<=100)dev_var.level = i;
                                else dev_var.level = 100;
                                break;
        default:;
    }
}
//============================================================================
uint8_t Params_prop( uint8_t par, uint8_t * name, uint8_t * prop, uint8_t * nbyte )
{
	char * str;
    uint8_t size = 0;
	if( name ) {switch( par )
        {
	case PARAM_DELTA       :	str ="Delta level (mS)"; break;
	case PARAM_BCOEF       :	str ="PWM coef"; break;
        default: return 0;
        }
        while( *str ) { *name++ = *str++; size++; } *name = 0; 
    }
	if( prop ) switch( par )
        {
    case PARAM_DELTA      :	*prop = UINT; break;
    case PARAM_BCOEF      :	*prop = REAL; break;
        default: return 0;
        }
	if( nbyte ) switch( par )
        {
    case PARAM_DELTA      :	*nbyte = 2; break;
    case PARAM_BCOEF      :	*nbyte = 4; break;
        default: return 0;
        }
    return size;
}
//============================================================================
void Params_get(uint8_t par, void * value)
{
    switch( par )
    {
		case PARAM_DELTA     	:	*(Uint32*)value = Setup.delta; break;
		case PARAM_BCOEF     	:	*(float*)value = Setup.pwm_coef; break;
        default:;
    }
}
//============================================================================
void Params_set(uint8_t par, void * value)
{
	u16 i = *(u16*)value;
	float f = *(float*)value;
    switch( par )
    {
		case PARAM_DELTA	  :	Setup.delta = i; break;
		case PARAM_BCOEF	  :	Setup.pwm_coef = f; break;
        default:;
    }
}//=========================================================================
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    
}
//=========================================================================
// Функция обработки
void CONTROL_Proc(void)
{
	static Uint32 timeout_type = 0;
	static Uint16 level_old = 0;

	if(dev_var.start == 0)
	{
        dev_var.start = 1;
	}

	if(timeout_type+Setup.delta <= HAL_GetTick())
	{
        timeout_type = HAL_GetTick();
        
        if(level_old != dev_var.level)
        {
            if(level_old < dev_var.level) level_old++;
            else if(level_old > dev_var.level) level_old--;
            u32 tmp = 65535;
            tmp = level_old*tmp/100;
            dev_var.pwm = tmp;
            TIM4->CCR2 = dev_var.pwm;
        } 
	}
}
//=========================================================================

