import { Component, OnDestroy, OnInit } from "@angular/core";
import { WebsocketService } from "../../services/websocket.service";

@Component({
  selector: "app-livedata",
  standalone: true,
  imports: [],
  templateUrl: "./livedata.component.html",
})
export class LivedataComponent implements OnInit, OnDestroy {
  // ***** FIELDS *****
  distance0: number = 0;
  distance1: number = 0;

  // ***** CONSTRUCTOR *****
  constructor(public websocket: WebsocketService) {}

  // ***** METHODS *****

  ngOnDestroy(): void {
    this.websocket.disconnect();
  }

  ngOnInit(): void {
    this.websocket.distance0.subscribe((value) => {
      this.distance0 = value;
    });
    this.websocket.distance1.subscribe((value) => {
      this.distance1 = value;
    });
  }

  connect(): void {
    this.websocket.connect();
  }

  disconnect(): void {
    this.websocket.disconnect();
  }

  submitForm() {
    throw new Error("Method not implemented.");
  }
}
