=begin
This is a ruby version of Jim Bublitz's pykde program, translated by Richard Dale
=end

=begin
A basic system tray application - you can combine this with code from
menuapp2.rb or menuapp3.rb to quickly build a full-blown application
framework.
=end

=begin
Copyright 2003 Jim Bublitz

Terms and Conditions

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to
deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KDE::IND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
COPYRIGHT HOLDER BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

Except as contained in this notice, the name of the copyright holder shall
not be used in advertising or otherwise to promote the sale, use or other
dealings in this Software without prior written authorization from the
copyright holder.
=end

require 'Korundum'

# This template uses KDE::MainWindow as the main window widget
# It solves the problem described in systray.rb by using
# a flag to control the return value from queryClose - if
# :quit" is signalled from anywhere EXCEPT the system tray
# icon's menu, @exitFlag == false, and as the return
# value for queryClose, it stops the application from shutting
# down; if @exitFlag is true, the application shuts down

class MainWin < KDE::MainWindow
	slots 'slotQuitSelected()'
	
    def initialize(*args)
        super

        @exitFlag = false

        icons = KDE::IconLoader.new()

        # KDE::SystemTray hides or shows its parent when the system tray icon is clicked
        systray = KDE::SystemTray.new(self)
        systray.setPixmap(icons.loadIcon("stop", 0))
        connect(systray, SIGNAL("quitSelected()"), self, SLOT('slotQuitSelected()'))
        systray.show()
	end

    # Controls whether or not the application really exits
    def queryClose()
        hide()
        return @exitFlag
	end

    # Receives the signal emitted when the user selects Quit from the
    # system tray icon's menu
    def slotQuitSelected()
        @exitFlag = true
        KDE::Application.kApplication().quit()
	end
end

#-------------------- main ------------------------------------------------

# The usual stuff..
appName = "template"
about = KDE::AboutData.new(appName, "A system tray", "0.1")
KDE::CmdLineArgs.init(ARGV, about)
app = KDE::Application.new()

mainWindow = MainWin.new(nil, "main window")

mainWindow.show
app.exec

