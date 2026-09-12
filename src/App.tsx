import { ReactNode } from "react";
import { BrowserRouter, Routes, Route, Navigate } from "react-router-dom";
import { AuthProvider, useAuth } from "./context/AuthContext";
import Login from "./pages/Login";
import GovernmentDashboard from "./pages/GovernmentDashboard";
import CreateProject from "./pages/CreateProject";
import StartupDashboard from "./pages/StartupDashboard";
import ProjectDetails from "./pages/ProjectDetails";
import Rankings from "./pages/Rankings";

function RequireAuth({ children }: { children: ReactNode }) {
  const { role } = useAuth();
  return role ? children : <Navigate to="/login" replace />;
}

export default function App() {
  return (
    <AuthProvider>
      <BrowserRouter>
        <Routes>
          <Route path="/login" element={<Login />} />

          <Route
            path="/government"
            element={<RequireAuth><GovernmentDashboard /></RequireAuth>}
          />
          <Route
            path="/government/projects/new"
            element={<RequireAuth><CreateProject /></RequireAuth>}
          />

          <Route
            path="/startup"
            element={<RequireAuth><StartupDashboard /></RequireAuth>}
          />

          <Route
            path="/projects/:id"
            element={<RequireAuth><ProjectDetails /></RequireAuth>}
          />
          <Route
            path="/projects/:id/rankings"
            element={<RequireAuth><Rankings /></RequireAuth>}
          />

          <Route path="*" element={<Navigate to="/login" replace />} />
        </Routes>
      </BrowserRouter>
    </AuthProvider>
  );
}
