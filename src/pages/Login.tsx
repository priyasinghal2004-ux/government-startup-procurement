import { useState, FormEvent } from "react";
import { useNavigate } from "react-router-dom";
import { apiFetch } from "../api/client";
import { useAuth } from "../context/AuthContext";

interface LoginResponse {
  token: string;
  userId: number;
  role: "government" | "startup";
}

export default function Login() {
  const [email, setEmail] = useState("");
  const [password, setPassword] = useState("");
  const [error, setError] = useState("");
  const { login } = useAuth();
  const navigate = useNavigate();

  async function handleSubmit(e: FormEvent) {
    e.preventDefault();
    setError("");
    try {
      const res = await apiFetch<LoginResponse>("/auth/login", {
        method: "POST",
        body: JSON.stringify({ email, password }),
      });
      login(res.token, res.userId, res.role);
      navigate(res.role === "government" ? "/government" : "/startup");
    } catch (err) {
      setError((err as Error).message || "Login failed");
    }
  }

  return (
    <div style={{ maxWidth: 360, margin: "80px auto", fontFamily: "sans-serif" }}>
      <h2>Login</h2>
      <p style={{ color: "#666", fontSize: 14 }}>
        Default credentials from migration:<br />
        Government: <code>gov@example.gov.in</code> / <code>password123</code><br />
        Startup: <code>aquatech@example.com</code> / <code>password123</code>
      </p>
      <form onSubmit={handleSubmit}>
        <input
          placeholder="Email"
          value={email}
          onChange={(e) => setEmail(e.target.value)}
          style={{ display: "block", width: "100%", marginBottom: 8, padding: 8, boxSizing: "border-box" }}
        />
        <input
          placeholder="Password"
          type="password"
          value={password}
          onChange={(e) => setPassword(e.target.value)}
          style={{ display: "block", width: "100%", marginBottom: 8, padding: 8, boxSizing: "border-box" }}
        />
        <button type="submit" style={{ padding: "8px 16px", cursor: "pointer" }}>Login</button>
        {error && <p style={{ color: "red", marginTop: 8 }}>{error}</p>}
      </form>
    </div>
  );
}
