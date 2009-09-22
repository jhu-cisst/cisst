# This file was obtained on the Internet. There do not appear to be any license restrictions
# (use as you wish)

import string

from wx import *

from ireTaskTree import *

try:
   from numpy.oldnumeric import *
except:
   print 'Numpy package is required'
   raise

from wx.lib.plot     import *
from wxSpinSequence  import *

class Cchannel:
    def __init__(self,parent,index,color,channelsizer):
        self.parent = parent
        self.index  = index
        self.color  = color
        self.readCommand = None
        self.readCommandSuffix = ''

        self.enable   = 1
        self.enableID = wx.NewId()
        self.enableTB = wx.ToggleButton(parent, self.enableID, _("Enabled"))
        EVT_TOGGLEBUTTON(parent, self.enableID, self.OnEnableToggle)

        self.sigID = wx.NewId()
        self.sigBut = wx.Button(parent, self.sigID, _("Signal"))
        EVT_BUTTON(parent, self.sigID, self.OnSignal)

        self.sizer = wx.BoxSizer(wx.HORIZONTAL)
        self.sizer.Add(self.sigBut,   1, wx.EXPAND|wx.ALL, 2)
        self.sizer.Add(self.enableTB, 2, wx.EXPAND|wx.ALL, 2)
                   
        self.gainLB = wx.StaticText(parent, wx.NewId(), _("Gain"),   style=wx.ALIGN_RIGHT)

        self.gain   = 1.0
        self.gainID = wx.NewId()
        self.gainSC = wxSpinSequence(parent,self.gainID,-1000.0,1000.0,self.gain,[1,2,5])
        EVT_TEXT(parent, self.gainID, self.OnGain)
        
        self.gainbox = wx.BoxSizer(wx.HORIZONTAL)
        self.gainbox.Add(self.gainLB, 1, wx.EXPAND|wx.ALL, 2)
        self.gainbox.Add(self.gainSC, 2, wx.EXPAND|wx.ALL, 2)
        
        self.offsLB = wx.StaticText(parent, wx.NewId(), _("Offset"), style=wx.ALIGN_RIGHT)

        self.offs   = index+1.0
        self.offsID = wx.NewId()
        self.offsSC = wxSpinSequence(parent,self.offsID,-1000.0,1000.0,self.offs,[1,2,5])
        EVT_TEXT(parent,self.offsID, self.OnOffs) 

        self.offsbox = wx.BoxSizer(wx.HORIZONTAL)
        self.offsbox.Add(self.offsLB, 1, wx.EXPAND|wx.ALL, 2)
        self.offsbox.Add(self.offsSC, 2, wx.EXPAND|wx.ALL, 2)            
        
        self.box = wx.StaticBoxSizer(wx.StaticBox(parent, -1, _("Channel ")+str(index)), wx.VERTICAL)
        self.box.Add(self.sizer,   0, wx.EXPAND|wx.ALL, 2)
        self.box.Add(self.gainbox, 0, wx.EXPAND|wx.ALL, 2)
        self.box.Add(self.offsbox, 0, wx.EXPAND|wx.ALL, 2)
        channelsizer.Add(self.box, 1, wx.EXPAND|wx.ALL, 2)
        

    def OnEnableToggle(self, event):
        if self.enable == 1:                   
            self.enable = 0
            self.enableTB.SetLabel(_("Disabled"))
            self.ydata = 1.0*zeros((self.parent.nMax,1))       
        else:
            self.enable = 1
            self.enableTB.SetLabel(_("Enabled"))
        
        self.parent.timer.mutex = 1
        self.parent.CreatePlotGraphics()
        self.parent.ResetPlotAxis()
        self.parent.ResetPlotData()
        self.parent.timer.mutex = 0
            

    def OnSignal(self, event):
        dlg = ireSignalSelect(self.parent, -1, "Choose signal", self.parent.taskManager)
        if dlg.ShowModal() == wx.ID_OK:
            self.SetReadCommand(dlg.GetSelection())
        dlg.Destroy()

    def OnGain(self,event):
        sc = event.GetEventObject()
        self.gain = string.atof(sc.GetValue())


    def OnOffs(self,event):
        sc = event.GetEventObject()
        self.offs = string.atof(sc.GetValue())

    def SetReadCommand(self, readCommand):
        self.readCommand = readCommand[0]
        self.readCommandSuffix = readCommand[1]


class COscilloscope(wx.Frame):
    def __init__ (self,frame,taskManager,nchannels=4):
        wx.Frame.__init__ (self,frame,-1,_("Oscilloscope"))
        self.SetSize((800, 600))

        self.taskManager = taskManager
        self.nMax      = 1000 
        self.status    = "idle"    
        if nchannels > 4: nchannels = 4   
        self.nchannels = nchannels       
        
        self.timerID = wx.NewId()
        self.timer   = wx.Timer(self,self.timerID)
        EVT_TIMER(self,self.timerID,self.OnTestTimer)      
        self.timer.mutex = 0
        
        self.valuerangeLB = wx.StaticText(self, wx.NewId(), _("Value Range"), style = wx.ALIGN_CENTRE)

        self.valuerangeID = wx.NewId()
        self.valuerangeSC = wxSpinSequence(self, self.valuerangeID, 0.001, 1000.0, self.nchannels+1, [1,2,5])
        EVT_TEXT(self, self.valuerangeID, self.OnValueSpinCtrl) 

        self.timerangeLB = wx.StaticText(self, wx.NewId(), _("Time Range [s]"), style = wx.ALIGN_CENTRE)

        self.timerangeID = wx.NewId()
        self.timerangeSC = wxSpinSequence(self, self.timerangeID, 1.0, 1000000.0, 2.0, [1,2,5])
        EVT_TEXT(self, self.timerangeID, self.OnTimeSpinCtrl) 
                 
        timerangesizer = wx.StaticBoxSizer(wx.StaticBox(self, -1, "Settings"), wx.VERTICAL)
        #PKAZ: following not valid
        #timerangesizer.Add(1, 1, 1, wx.EXPAND)
        timerangesizer.Add(self.valuerangeLB, 0, wx.ALIGN_BOTTOM|wx.ALL, 2)
        timerangesizer.Add(self.valuerangeSC, 0, wx.ALIGN_BOTTOM|wx.ALL, 2)
        timerangesizer.Add(self.timerangeLB,  0, wx.ALIGN_BOTTOM|wx.ALL, 2)
        timerangesizer.Add(self.timerangeSC,  0, wx.ALIGN_BOTTOM|wx.ALL, 2)
        
        channelsizer = wx.BoxSizer(wx.HORIZONTAL)
        channelsizer.Add(timerangesizer, 0, wx.EXPAND|wx.ALL, 2)

        self.colors   = ['red','blue','green','cyan']
        self.channels = []
        for index in range(0,self.nchannels):
            color   = self.colors[index]
            channel = Cchannel(self,index,color,channelsizer)
            self.channels.append(channel)

        self.plot = PlotCanvas(self)
        self.ResetTimeData()
        self.ResetChartData()
        self.ResetPlotData()
        self.CreatePlotGraphics()
        self.ResetPlotAxis()
       
        #Create mouse event for showing cursor coords in status bar
        #
        EVT_LEFT_DOWN(self.plot, self.OnMouseLeftDown)
        
        startbuttonID = wx.NewId()
        startbutton   = wx.Button(self, startbuttonID, _("Start"))
        EVT_BUTTON(self, startbuttonID, self.OnTestStart )        

        stopbuttonID  = wx.NewId()
        stopbutton    = wx.Button(self, stopbuttonID,  _("Stop"))
        EVT_BUTTON(self, stopbuttonID,  self.OnTestStop  )        

        closebuttonID = wx.NewId()
        closebutton   = wx.Button(self, closebuttonID, _("Close"))
        EVT_BUTTON(self, closebuttonID, self.OnClose )        

        leiste = wx.BoxSizer(wx.HORIZONTAL)
        leiste.Add(startbutton, 1, wx.EXPAND|wx.ALL, 10)
        leiste.Add(stopbutton,  1, wx.EXPAND|wx.ALL, 10)
        leiste.Add(closebutton, 1, wx.EXPAND|wx.ALL, 10)

        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(channelsizer, 0, wx.EXPAND|wx.ALL, 2)
        sizer.Add(self.plot,    1, wx.EXPAND|wx.ALL, 2)
        sizer.Add(leiste,       0, wx.EXPAND|wx.ALL, 2)

        self.SetSizer(sizer)

        # Now Create the menu bar and items
        #
        self.mainmenu = wx.MenuBar()

        menu = wx.Menu()
        menu.Append(200, 'Page Setup...', 'Setup the printer page')
        EVT_MENU(self, 200, self.OnFilePageSetup)
        
        menu.Append(201, 'Print Preview...', 'Show the current plot on page')
        EVT_MENU(self, 201, self.OnFilePrintPreview)
        
        menu.Append(202, 'Print...', 'Print the current plot')
        EVT_MENU(self, 202, self.OnFilePrint)
        
        menu.Append(203, 'Save Plot...', 'Save current plot')
        EVT_MENU(self, 203, self.OnSaveFile)
        
        menu.Append(205, 'E&xit', 'Enough of this already!')
        EVT_MENU(self, 205, self.OnFileExit)

        self.mainmenu.Append(menu, '&File')

        menu = wx.Menu()
        menu.Append(207, 'Start', 'Start moving plot')
        EVT_MENU(self,207,self.OnTestStart)

        menu.Append(209, 'Stop', 'Stop moving plot')
        EVT_MENU(self,209,self.OnTestStop)
         
        menu.Append(211, '&Redraw', 'Redraw plots')
        EVT_MENU(self,211,self.OnPlotRedraw)

        menu.Append(212, '&Clear', 'Clear canvas')
        EVT_MENU(self,212,self.OnPlotClear)

        menu.Append(213, '&Scale', 'Scale canvas')
        EVT_MENU(self,213,self.OnPlotScale) 

        menu.Append(214, 'Enable &Zoom', 'Enable Mouse Zoom', kind=wx.ITEM_CHECK)
        EVT_MENU(self,214,self.OnEnableZoom) 

        menu.Append(215, 'Enable &Grid', 'Turn on Grid', kind=wx.ITEM_CHECK)
        EVT_MENU(self,215,self.OnEnableGrid)

        menu.Append(220, 'Enable &Legend', 'Turn on Legend', kind=wx.ITEM_CHECK)
        EVT_MENU(self,220,self.OnEnableLegend) 

        menu.Append(225, 'Scroll Up 1', 'Move View Up 1 Unit')
        EVT_MENU(self,225,self.OnScrUp) 

        menu.Append(230, 'Scroll Rt 2', 'Move View Right 2 Units')
        EVT_MENU(self,230,self.OnScrRt)

        menu.Append(235, '&Plot Reset', 'Reset to original plot')
        EVT_MENU(self,235,self.OnReset)

        self.mainmenu.Append(menu, '&Plot')

        menu = wx.Menu()
        menu.Append(300, '&About', 'About this thing...')
        EVT_MENU(self, 300, self.OnHelpAbout)

        self.mainmenu.Append(menu, '&Help')

        self.SetMenuBar(self.mainmenu)

        # A status bar to tell people what's happening
        #
        self.CreateStatusBar(1)

        EVT_CLOSE(self, self.OnClose)

        self.Show(True)
        

    def OnMouseLeftDown(self,event):
        s= "Left Mouse Down at Point: (%.4f, %.4f)" % self.plot.GetXY(event)
        self.SetStatusText(s)
        event.Skip()

    def OnFilePageSetup(self, event):
        self.plot.PageSetup()
        
    def OnFilePrintPreview(self, event):
        self.plot.PrintPreview()
        
    def OnFilePrint(self, event):
        self.plot.Printout()
        
    def OnSaveFile(self, event):
        self.plot.SaveFile()

    def OnFileExit(self, event):
        self.Close()

    def OnPlotDraw1(self, event):
        self.resetDefaults()
        self.plot.Draw(_draw1Objects())
    
    def OnPlotRedraw(self,event):
        self.plot.Redraw()

    def OnPlotClear(self,event):
        self.plot.Clear()
        
    def OnPlotScale(self, event):
        if self.plot.last_draw != None:
            graphics, xAxis, yAxis= self.plot.last_draw
            self.plot.Draw(graphics,(1,3.05),(0,1))

    def OnEnableZoom(self, event):
        self.plot.SetEnableZoom(event.IsChecked())
        
    def OnEnableGrid(self, event):
        self.plot.SetEnableGrid(event.IsChecked())
        
    def OnEnableLegend(self, event):
        self.plot.SetEnableLegend(event.IsChecked())

    def OnScrUp(self, event):
        self.plot.ScrollUp(1)
        
    def OnScrRt(self,event):
        self.plot.ScrollRight(2)

    def OnReset(self,event):
        self.plot.Reset()

    def OnHelpAbout(self, event):
        about = wx.MessageDialog(self, "About", "About...")
        about.ShowModal()

    def resetDefaults(self):
        """Just to reset the fonts back to the PlotCanvas defaults"""
        self.plot.SetFont(wx.Font(10,wx.SWISS,wx.NORMAL,wx.NORMAL))
        self.plot.SetFontSizeAxis(10)
        self.plot.SetFontSizeLegend(7)
        self.plot.SetXSpec('auto')
        self.plot.SetYSpec('auto')


    def OnClose(self,event):
        self.OnTestStop(event)
        self.Destroy()


    def ResetTimeData(self):
        T = string.atof(self.timerangeSC.GetValue())
        s = [1,2,5]
        i =  0 # dT_0 = 0.1
        p = -1 # dT_0 = 0.1
        while(1):            
            dT = s[i]*pow(10,p)
            n  = int(T/dT)
            if n <= self.nMax:
                break
            i += 1
            if i == len(s):
                i  = 0 
                p += 1
        self.n     = n
        self.dT    = dT
        # Create an array 1..(n-1) for the time (x axis)
        self.xdata = self.dT*arange(self.n)


    def ResetChartData(self):
        for channel in self.channels:
            channel.ydata = 1.0*zeros((self.nMax,1))


    def ResetPlotData(self):
        for channel in self.channels:
            channel.pdata = 1.0*zeros((self.n,2))
            channel.pdata[:,0] = self.xdata
            channel.pdata[:,1] = channel.ydata[-self.n:,0]  * channel.gain + channel.offs


    def ResetPlotAxis(self):
        self.xAxis = (min(self.xdata), max(self.xdata)) 
        self.yAxis = (0, string.atof(self.valuerangeSC.GetValue()))
        print 'ResetPlotAxis: ', self.yAxis
        if self.pg == None:
            self.plot.Clear()
        else:
            self.plot.Draw(self.pg, self.xAxis, self.yAxis)
    

    def CreatePlotGraphics(self):
        lines = []
        for channel in self.channels:
            if channel.enable == 0:
                continue

            channel.pline = PolyLine(channel.pdata, legend="Channel", colour=channel.color)
            lines.append(channel.pline)

        if len(lines) == 0:
            self.pg = None
            return
            
        self.pg = PlotGraphics(lines, "Signals", "Time [s]", "Values")


    def OnTestStart(self,event):
        if self.status == "run":
            return

        self.ResetTimeData()
        self.ResetChartData()
        self.ResetPlotData()
        self.CreatePlotGraphics()
        self.ResetPlotAxis()

        self.time  = 0
        self.ResetTimer()


    def ResetTimer(self):
        doit = 0
        for channel in self.channels:
            if channel.enable == 1: doit +=1
        if doit == 0: return
            
        self.status = "run"
        self.timer.mutex = 0
        self.timer.Start(int(self.dT*1000)) # in ms


    def StopTimer(self):
        self.timer.Stop()
        self.status = "idle"    


    def OnTestTimer(self,event):
        self.time += self.dT

        if self.timer.mutex == 1:
            return
            
        for channel in self.channels:
            y = 0
            if channel.readCommand:
                y = channel.readCommand()
                if channel.readCommandSuffix:
                    y = eval('y'+channel.readCommandSuffix)

            # shift left and append y to end of ydata array
            channel.ydata[:-1] = channel.ydata[1:]
            channel.ydata[-1]  = y

            if channel.enable == 0:
                continue

            channel.pdata[:,1] = channel.ydata[-self.n:,0]  * channel.gain + channel.offs            
            channel.pline.points = channel.pdata

        if self.pg == None:
            self.plot.Clear()
        else:
            self.plot.Draw(self.pg)
        wx.GetApp().Yield(True)
        

    def OnTestStop(self,event):
        if self.status == "run":
            self.StopTimer()


    def OnValueSpinCtrl(self,event):
        self.timer.mutex = 1
        self.ResetPlotAxis()
        self.timer.mutex = 0
        

    def OnTimeSpinCtrl(self,event):
        if self.status == "run":
            self.StopTimer()
            self.ResetTimeData()
            self.ResetPlotData()
            self.ResetPlotAxis()
            self.ResetTimer()
        else:
            self.ResetTimeData()
            self.ResetPlotData()
            self.ResetPlotAxis()

#    def AddSignal(self, chan, readCommand):
#       if chan >= 0 and chan < 4:
#           self.channels[chan].SetReadCommand(readCommand)
#       else:
#           print 'Channel number ', chan, ' out of range (0-3)'

 
class MyApp(wx.App):
    def OnInit(self):
        frame = COscilloscope(None)
        frame.Show(True)
        self.SetTopWindow(frame)
        return True


if __name__ == '__main__':
    import gettext
    gettext.install("dm")
    app = MyApp(0)
    app.MainLoop()
