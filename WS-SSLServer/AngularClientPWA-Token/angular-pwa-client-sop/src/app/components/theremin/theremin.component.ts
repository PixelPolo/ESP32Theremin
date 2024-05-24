import { Component, OnDestroy } from "@angular/core";
import { WebsocketService } from "../../services/websocket.service";

@Component({
  selector: "app-theremin",
  standalone: true,
  templateUrl: "./theremin.component.html",
  imports: [],
})
export class ThereminComponent implements OnDestroy {
  // ***** FIELDS *****
  private audioCtx: AudioContext | undefined;
  public oscillator: OscillatorNode | undefined;
  public gainNode: GainNode | undefined;
  isConnected: boolean = false;

  // ***** CONSTRUCTOR *****
  constructor(public websocket: WebsocketService) {
    if (typeof window !== "undefined") {
      this.audioCtx = new AudioContext();
    }

  }

  // ***** METHODS *****

  // ***** Method to adjust frequency *****

  mapDistanceToFrequency(distance: number): number {
    const minFrequency = 10;
    const maxFrequency = 80;
    const scaledFrequency =
      minFrequency + (maxFrequency - minFrequency) * (distance / 100);
    if (scaledFrequency > 500) {
      return maxFrequency;
    } else {
      return scaledFrequency;
    }
  }

  setFrequency(frequency: number): void {
    if (this.oscillator && this.audioCtx) {
      const now = this.audioCtx.currentTime;
      this.oscillator.frequency.linearRampToValueAtTime(frequency, now + 0.5);
    }
  }

  // ***** Method to adjust frequency *****

  mapDistanceToVolume(distance: number): number {
    if (distance < 500) {
      return distance / 100;
    } else {
      return 0.1;
    }
  }

  setVolume(volume: number): void {
    if (this.gainNode && this.audioCtx) {
      const now = this.audioCtx.currentTime;
      this.gainNode.gain.linearRampToValueAtTime(volume, now + 0.2);
    }
  }

  // ***** Method to create an Oscillator with a gain *****

  createOscillator(): void {
    if (this.audioCtx) {
      this.oscillator = this.audioCtx.createOscillator();
      this.gainNode = this.audioCtx.createGain();
      this.oscillator.connect(this.gainNode);
      this.gainNode.connect(this.audioCtx.destination);
      this.oscillator.start();
    }
  }

  // ***** Connect the websocket, subscribte to distances, create oscillator *****

  connect(): void {
    this.websocket.connect();
    // Subscribe to distance0 for frequency
    this.websocket.distance0.subscribe((distance0) => {
      const frequency = this.mapDistanceToFrequency(distance0);
      this.setFrequency(frequency);
      console.log("Frequency: ", frequency);
    });
    // Subscribe to distance1 for volume
    this.websocket.distance1.subscribe((distance1) => {
      const volume = this.mapDistanceToVolume(distance1);
      this.setVolume(volume);
      console.log("Volume adjusted: ", volume);
    });
    // Delay to let the websocket connect and show On Air !
    setTimeout(() => {
      this.isConnected = true;
      this.createOscillator(); // Create an oscillator
      this.setVolume(0.1); // Set volume to 0.1
    }, 5000);
  }

  // ***** Dispose oscillator and gain *****

  disposeSound() {
    if (this.oscillator && this.gainNode) {
      this.oscillator.stop();
      this.oscillator.disconnect();
      this.gainNode?.disconnect();
      this.oscillator = undefined;
      this.gainNode = undefined;
    }
  }

  // ***** Disconnect the websocket and dispose sound *****

  disconnect(): void {
    this.disposeSound();
    this.websocket.disconnect();
    this.isConnected = false;
  }

  // ***** Disconnect and close audio context *****

  ngOnDestroy(): void {
    this.disconnect();
    this.audioCtx?.close(); // close audioCtx
  }
}
