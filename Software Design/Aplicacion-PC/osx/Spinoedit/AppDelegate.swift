//
//  AppDelegate.swift
//
//  Created by Guillaume Chereau on 10/16/15.
//  Copyright © 2015 Noctua Software Limited. All rights reserved.
//

import Cocoa
import OpenGL
import GLKit
import AppKit
import Foundation
import IOBluetooth
import IOBluetoothUI

class SpinoNSOpenGLView: NSOpenGLView {
    override func awakeFromNib()
    {
        let attr = [
            NSOpenGLPixelFormatAttribute(NSOpenGLPFAOpenGLProfile),
            NSOpenGLPixelFormatAttribute(NSOpenGLProfileVersionLegacy),
            NSOpenGLPixelFormatAttribute(NSOpenGLPFAColorSize), 24,
            NSOpenGLPixelFormatAttribute(NSOpenGLPFAAlphaSize), 8,
            NSOpenGLPixelFormatAttribute(NSOpenGLPFADoubleBuffer),
            NSOpenGLPixelFormatAttribute(NSOpenGLPFADepthSize), 32,
            0
        ]
        
        let format = NSOpenGLPixelFormat(attributes: attr)
        let context = NSOpenGLContext(format: format!, share: nil)
        
        self.openGLContext = context
        self.openGLContext?.makeCurrentContext()
        self.window?.acceptsMouseMovedEvents = true
    }
    
    func appDelegate () -> AppDelegate
    {
        return NSApplication.shared().delegate as! AppDelegate
    }
    
    override var acceptsFirstResponder: Bool {
        return true
    }
    
    func mouseEvent(_ id: Int, _ state: Int, _ event: NSEvent) {
        appDelegate().inputs.mouse_pos.0 = Float(event.locationInWindow.x);
        appDelegate().inputs.mouse_pos.1 = Float(self.frame.height - event.locationInWindow.y);
        
        // XXX: find a way to make it work with unsage memory.
        switch (id) {
        case 0: appDelegate().inputs.mouse_down.0 = (state != 0);
        case 1: appDelegate().inputs.mouse_down.1 = (state != 0);
        case 2: appDelegate().inputs.mouse_down.2 = (state != 0);
        default: break;
        }
    }
    
    override func mouseDown(with event: NSEvent)         { mouseEvent(0, 1, event) }
    override func mouseUp(with event: NSEvent)           { mouseEvent(0, 0, event) }
    override func mouseDragged(with event: NSEvent)      { mouseEvent(0, 2, event) }
    override func mouseMoved(with event: NSEvent)        { mouseEvent(0, 0, event) }
    override func rightMouseDown(with event: NSEvent)    { mouseEvent(2, 1, event) }
    override func rightMouseUp(with event: NSEvent)      { mouseEvent(2, 0, event) }
    override func rightMouseDragged(with event: NSEvent) { mouseEvent(2, 1, event) }
    override func otherMouseDown(with event: NSEvent)    { mouseEvent(1, 1, event) }
    override func otherMouseUp(with event: NSEvent)      { mouseEvent(1, 0, event) }
    override func otherMouseDragged(with event: NSEvent) { mouseEvent(1, 1, event) }
    
    override func scrollWheel(with event: NSEvent) {
        appDelegate().inputs.mouse_wheel = Float(event.scrollingDeltaY)
    }
    
    override func keyDown(with event: NSEvent) {
        switch (event.keyCode) {
        case 51: appDelegate().inputs.keys.259 = true
        case 123: appDelegate().inputs.keys.263 = true
        case 124: appDelegate().inputs.keys.262 = true
        case 125: appDelegate().inputs.keys.264 = true
        case 126: appDelegate().inputs.keys.265 = true
        default: break
        }
        let scalars = event.characters!.unicodeScalars
        let c = scalars[scalars.startIndex].value
        if (c < 256) {
            appDelegate().inputs.chars.0 = c
        }
    }
    
    override func keyUp(with event: NSEvent) {
        switch (event.keyCode) {
        case 51: appDelegate().inputs.keys.259 = false
        case 123: appDelegate().inputs.keys.263 = false
        case 124: appDelegate().inputs.keys.262 = false
        case 125: appDelegate().inputs.keys.264 = false
        case 126: appDelegate().inputs.keys.265 = false
        default: break
        }
    }

    override func flagsChanged(with event: NSEvent) {
        appDelegate().inputs.keys.340 = event.modifierFlags.contains(.shift)
    }
}


@NSApplicationMain
class AppDelegate: NSObject, NSApplicationDelegate, IOBluetoothRFCOMMChannelDelegate {
    
    @IBOutlet weak var window: NSWindow!
    @IBOutlet weak var view: SpinoNSOpenGLView!
    fileprivate var timer: Timer!
    
    open var spinoedit = spinoedit_t()
    open var inputs = inputs_t()

    var mRFCOMMChannel : IOBluetoothRFCOMMChannel?

    func applicationDidFinishLaunching(_ aNotification: Notification) {
        timer = Timer(timeInterval: 1.0 / 60.0,
                      target: self,
                      selector: #selector(AppDelegate.onTimer(_:)),
                      userInfo: nil,
                      repeats: true)
        RunLoop.current.add(timer, forMode: RunLoopMode.defaultRunLoopMode)
        spinoedit_init(&self.spinoedit)

        bluetooth_set_connect_callback {
            let appDelegate = NSApplication.shared().delegate as! AppDelegate
            return appDelegate.bt_connect()
        }
        bluetooth_set_send_callback {
            (data: UnsafeRawPointer?, size: Int32) -> Int32 in
            let appDelegate = NSApplication.shared().delegate as! AppDelegate
            return appDelegate.bt_write(UnsafeMutableRawPointer(mutating: data), size)
        }
    }

    func bt_connect() -> Int32 {
        guard let deviceSelector = IOBluetoothDeviceSelectorController.deviceSelector() else {
            return -1
        }
        let sppServiceUUID = IOBluetoothSDPUUID.uuid32(kBluetoothSDPUUID16ServiceClassSerialPort.rawValue)
        deviceSelector.addAllowedUUID(sppServiceUUID)
        if ( deviceSelector.runModal()  !=  Int32(kIOBluetoothUISuccess) ) {
            return -1
        }
        guard let deviceArray = deviceSelector.getResults() else {
            return -1
        }
        if (deviceArray.count == 0) {
            return -1;
        }
        let device: IOBluetoothDevice = deviceArray[0] as! IOBluetoothDevice;
        guard let sppServiceRecord = device.getServiceRecord(for: sppServiceUUID) else {
            return -1
        }
        var rfcommChannelID: BluetoothRFCOMMChannelID = 0
        if (sppServiceRecord.getRFCOMMChannelID(&rfcommChannelID) != kIOReturnSuccess ) {
            return -1
        }
        if ( device.openRFCOMMChannelSync(&mRFCOMMChannel, withChannelID: rfcommChannelID, delegate: self) != kIOReturnSuccess) {
            return -1
        }
        mRFCOMMChannel?.setDelegate(self)
        return 0
    }

    func bt_write(_ data: UnsafeMutableRawPointer?, _ size: Int32) -> Int32 {
        assert(mRFCOMMChannel != nil)
        let send_size = min(size, 64)
        let ret = mRFCOMMChannel?.writeSync(data, length: UInt16(send_size));
        return ret == kIOReturnSuccess ? send_size : -1;
    }

    func rfcommChannelControlSignalsChanged(_ rfcommChannel: IOBluetoothRFCOMMChannel!) {
        print("channelcontrolsignalschanged")
    }

    func rfcommChannelQueueSpaceAvailable(_ rfcommChannel: IOBluetoothRFCOMMChannel!) {
        print("queuespaceavailable")
    }

    func rfcommChannelClosed(_ rfcommChannel: IOBluetoothRFCOMMChannel!) {
        print("closed")
    }

    func rfcommChannelFlowControlChanged(_ rfcommChannel: IOBluetoothRFCOMMChannel!) {
        print("flowcontrolchanged")
    }

    func rfcommChannelWriteComplete(_ rfcommChannel: IOBluetoothRFCOMMChannel!, refcon: UnsafeMutableRawPointer!, status error: IOReturn) {
        print("write complete")
    }

    func rfcommChannelData(_ rfcommChannel: IOBluetoothRFCOMMChannel!, data: UnsafeMutableRawPointer!, length dataLength: Int) {
        bluetooth_signal(1, 1)
    }

    func rfcommChannelOpenComplete(_ rfcommChannel: IOBluetoothRFCOMMChannel!, status error: IOReturn) {
        if(error != kIOReturnSuccess){
            print("Error - Failed to open the RFCOMM channel");
        }
        else {
            print("Connected");
            bluetooth_signal(0, 0);
        }
    }

    func applicationWillTerminate(_ aNotification: Notification) {
        // Insert code here to tear down your application
    }
    func applicationShouldTerminateAfterLastWindowClosed(_ sender: NSApplication) -> Bool {
        return true
    }
    func onTimer(_ sender: Timer!) {
        if (!window.isVisible) {
            return
        }
        glClear(GLbitfield(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT))
        self.inputs.window_size = (Int32(self.view.frame.size.width),
                                   Int32(self.view.frame.size.height))
        spinoedit_iter(&self.spinoedit, &self.inputs)
        spinoedit_render(&self.spinoedit)
        self.inputs.mouse_wheel = 0
        self.inputs.chars.0 = 0
        glFlush()
        view.openGLContext?.flushBuffer()
    }
}
