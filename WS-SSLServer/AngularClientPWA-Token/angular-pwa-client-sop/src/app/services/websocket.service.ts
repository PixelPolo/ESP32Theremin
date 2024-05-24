import { Injectable } from "@angular/core";
import { WebSocketSubject, webSocket } from "rxjs/webSocket";
import { Subject } from "rxjs";

@Injectable({
  providedIn: "root",
})
export class WebsocketService {
  // ***** FIELDS *****

  // Websocket Subject
  private subject: WebSocketSubject<any> | undefined;
  private url: string = "...";
  private authToken: string = "...";
  private serverUrl: string = `wss://${this.url}/?token=${this.authToken}`;

  // Distances as Observables subject
  private distance0Subject: Subject<number> = new Subject<number>();
  private distance1Subject: Subject<number> = new Subject<number>();

  // ***** CONSTRUCTOR *****
  constructor() {}

  // ***** METHODS *****

  // Method to attempt WebSocket connexion
  connect(): void {
    console.log("Connect !");

    // Construire l'URL du serveur WebSocket
    this.subject = webSocket(this.serverUrl);
    this.subject.subscribe({
      next: (msg) => {
        console.log("Received data:", msg);
        // Observables emit next value
        this.distance0Subject.next(msg.sensor1);
        this.distance1Subject.next(msg.sensor2);
      },
      error: (err) => console.log(err),
      complete: () => console.log("Connection complete"),
    });
  }

  // Method to complete (disconnect) websocket
  disconnect(): void {
    console.log("Disconnect !");
    if (this.subject != undefined) {
      this.subject.complete();
    }
  }

  // Getter for distance0 as an observable
  get distance0() {
    return this.distance0Subject.asObservable();
  }

  // Getter for distance1 as an observable
  get distance1() {
    return this.distance1Subject.asObservable();
  }
}
