import { Routes } from "@angular/router";
import { ThereminComponent } from "./components/theremin/theremin.component";
import { LivedataComponent } from "./components/livedata/livedata.component";
import { HomeComponent } from "./components/home/home.component";

export const routes: Routes = [
  // ***** ROUTES *****
  { path: "home", title: "Home", component: HomeComponent },
  { path: "theremin", title: "Theremin", component: ThereminComponent },
  { path: "livedata", title: "Live Data", component: LivedataComponent },
  { path: "", redirectTo: "home", pathMatch: "full" },
];
