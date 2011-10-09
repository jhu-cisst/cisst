# This file was obtained on the Internet. There do not appear to be any license restrictions
# (use as you wish)

#------------------------------------------------------
# Import what we need from the wx toolkit
#------------------------------------------------------
try:
    import wx
except Exception, e:
    print "Could not load wxPython: ", e
    sys.exit(1)

import string
from math import *

class wxSpinSequence(wx.Panel):
    def __init__(self,parent,ID=-1,minval=0.0,maxval=1.0,initial=0.0,sequence=None, N=2):
        wx.Panel.__init__(self, parent, wx.NewId(), wx.DefaultPosition, style=wx.SUNKEN_BORDER)
        
        Box = wx.BoxSizer(wx.HORIZONTAL)
        
        self.N   = N        # power for resolution [0.0, 1.0], such as 2 for 0.01
        self.max = maxval
        self.min = minval

        if sequence is None:
            self.sequence = [1,2,3,4,5,6,7,8,9]
        else:
            self.sequence = sequence
        
        index, power, sign = self.MakeIndexPowerSign(initial)
        value = self.MakeValue(index, power, sign)
                            
        if ID == -1: ID = wx.NewId()

        self.text = wx.TextCtrl(self,ID, str(value), wx.DefaultPosition, wx.DefaultSize, 
            style=wx.TE_RIGHT|wx.TE_READONLY )
        
        self.SBID = wx.NewId()
        self.SB   = wx.SpinButton(self, self.SBID,wx.DefaultPosition,wx.DefaultSize, wx.SP_HORIZONTAL)
        self.SB.SetRange(-1000000,1000000)
        self.SB.SetValue(0)
        wx.EVT_SPIN_UP(self, self.SBID, self.SpinUp)
        wx.EVT_SPIN_DOWN(self, self.SBID, self.SpinDown)

        Box.Add(self.text, 1, wx.EXPAND|wx.ALIGN_CENTER, 0)
        Box.Add(self.SB,   0, wx.ALIGN_CENTER, 0)

        self.SetAutoLayout(True)
        self.SetSizer(Box)
        self.Fit()


    def SpinUp(self,event):
        value = string.atof(self.text.GetValue())
        index, power, sign = self.MakeIndexPowerSign(value)
        N = len(self.sequence)

        if sign == 1:
            index+=1            # next in sequence UP
            if index > N-1:     # wrap around highest
                index  = 0      # and take lowest
                power += 1      # but increase power
            value = self.MakeValue(index, power, sign)

        elif sign == -1:          
            index-=1            # next in sequence UP
            if index < 0:       # wrap around highest
                index  = 0      # and take lowest
                power -= 1      # but increase power
            if power < -self.N: 
                value = 0.0
            else:
                value = self.MakeValue(index, power, sign)

        else: #sign == 0:
            value = self.MakeValue(0, -self.N, 1)

        if value <= self.max:
            self.text.SetValue(str(value))


    def SpinDown(self,event):
        value = string.atof(self.text.GetValue())
        index, power, sign = self.MakeIndexPowerSign(value)
        N = len(self.sequence)

        if sign == 1:          
            index-=1            # next in sequence DOWN
            if index < 0:       # wrap around lowest
                index  = N-1    # and take highest
                power -= 1      # but decrease power
            if power < -self.N: 
                value = 0.0
            else:
                value = self.MakeValue(index, power, sign)

        elif sign == -1:          
            N = len(self.sequence)
            index+=1            # next in sequence DOWN
            if index > N-1:     # wrap around lowest
                index  = 0      # and take highest
                power += 1      # but decrease power
            value = self.MakeValue(index, power, sign)

        else: #sign == 0:
            value = self.MakeValue(0, -self.N, -1)

        if value >= self.min:
            self.text.SetValue(str(value))
       

    def MakeIndexPowerSign(self,z):
        if   z > 0.0: s =  1
        elif z < 0.0: s = -1
        else: 
            return 0, 0, 0

        x = "%.0e" % abs(z)
        r = string.atoi(x[0])
        n = string.atoi(x[2:])
        N = len(self.sequence)
        for i in range(0,N):
            if r <= self.sequence[i]:   # first digit closest or equal to value in sequence
                break
        return i, n, s


    def MakeValue(self,i,n,s):
        if s == 0.0:
            return 0.0
        return s*self.sequence[i]*pow(10,n) # sequence[i] * 10^n


    def GetValue(self):
        return self.text.GetValue()

        
if __name__ == '__main__':
    class TestFrame(wx.Frame):
        def __init__(self):
            wx.Frame.__init__ (self, None, -1, "Sequence Spinner")

            text1 = wx.StaticText(self, -1, "Sequence Spinner, \ns=[1,2,3,4,5,6,7,8,9], N=2")

            ID = wx.NewId()
            spin1 = wxSpinSequence(self, ID, -10, 100.0)
            EVT_TEXT(self,ID, self.OnSpinSequence) 

            text2 = wx.StaticText(self, -1, "Sequence Spinner, \ns=[1,2,5], N=4")

            ID = wx.NewId()
            spin2 = wxSpinSequence(self, ID, -10, 100.0,0.0,[1,2,5],4)
            EVT_TEXT(self,ID, self.OnSpinSequence) 
            
            sizer = wx.BoxSizer(wx.VERTICAL)
            sizer.Add(text1, 0, wx.EXPAND|wx.ALIGN_LEFT, 0)
            sizer.Add(spin1, 0, wx.EXPAND|wx.ALIGN_LEFT, 0)            
            sizer.Add(text2, 0, wx.EXPAND|wx.ALIGN_LEFT, 0)
            sizer.Add(spin2, 0, wx.EXPAND|wx.ALIGN_LEFT, 0)            
            self.SetSizer(sizer)
            self.Fit()

        def OnSpinSequence(self,event):
            spin  = event.GetEventObject()
            value = spin.GetValue()
            print "OnSpinSequence, value = %s" % value
        
        def OnCloseWindow(self, event):
            self.Destroy()

    app = wxPySimpleApp()
    frame = TestFrame()
    frame.Show(True)
    app.MainLoop()
