#include "BCPhisicalScene.h"

BCPhisMaterialCollisionCharacter::BCPhisMaterialCollisionCharacter()
{// ������ ������� � ������� ������� � ODE
      m_Default.mode =dContactSlip1|dContactSlip2|
		      dContactSoftERP|dContactSoftCFM|dContactApprox1;
      m_Default.mu   =dInfinity;
      m_Default.slip1=0.1;
      m_Default.slip2 = 0.1;
      m_Default.soft_erp = 0.8;
      m_Default.soft_cfm = 0.01; // �� ������������� CFM==0.0. ������ ������ ����� ����������
}

BCPhisMaterialCollisionCharacter::~BCPhisMaterialCollisionCharacter()
{
}


