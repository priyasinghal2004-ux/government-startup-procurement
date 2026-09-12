import { createContext, useContext, useState, ReactNode } from "react";

interface AuthState {
  userId: number | null;
  role: "government" | "startup" | null;
}

interface AuthContextValue extends AuthState {
  login: (token: string, userId: number, role: "government" | "startup") => void;
  logout: () => void;
}

const AuthContext = createContext<AuthContextValue | undefined>(undefined);

export function AuthProvider({ children }: { children: ReactNode }) {
  const [state, setState] = useState<AuthState>({
    userId: Number(localStorage.getItem("userId")) || null,
    role: (localStorage.getItem("role") as AuthState["role"]) || null,
  });

  function login(token: string, userId: number, role: "government" | "startup") {
    localStorage.setItem("token", token);
    localStorage.setItem("userId", String(userId));
    localStorage.setItem("role", role);
    setState({ userId, role });
  }

  function logout() {
    localStorage.clear();
    setState({ userId: null, role: null });
  }

  return (
    <AuthContext.Provider value={{ ...state, login, logout }}>
      {children}
    </AuthContext.Provider>
  );
}

export function useAuth() {
  const ctx = useContext(AuthContext);
  if (!ctx) throw new Error("useAuth must be used inside AuthProvider");
  return ctx;
}
