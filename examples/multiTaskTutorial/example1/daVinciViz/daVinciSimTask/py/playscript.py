
import cisstCommonPython as cmn 
import cisstVectorPython as vct
import cisstMultiTaskPython as mts

lcm = mts.mtsManagerLocal_GetInstance()
lcm.GetNamesOfComponents()
disp = lcm.GetComponent('DISP:DISPProxy')
disp_provided = disp.GetProvidedInterface('Provided')
GetDouble = disp_provided.GetCommandRead('GetDouble')
a = GetDouble()
a.Data
disp_provided.GetNamesOfCommands()
ZeroAll = disp_provided.GetCommandVoid('ZeroAll')
