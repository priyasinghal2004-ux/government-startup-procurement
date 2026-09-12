const BASE_URL = import.meta.env.VITE_API_URL || "http://localhost:8080/api";

function getToken(): string | null {
  return localStorage.getItem("token");
}

export async function apiFetch<T>(
  path: string,
  options: RequestInit = {}
): Promise<T> {
  const token = getToken();

  const res = await fetch(`${BASE_URL}${path}`, {
    ...options,
    headers: {
      "Content-Type": "application/json",
      ...(token ? { Authorization: `Bearer ${token}` } : {}),
      ...(options.headers || {}),
    },
  });

  if (!res.ok) {
    const text = await res.text();
    throw new Error(text || `Request failed: ${res.status}`);
  }

  // Some endpoints (e.g. publish) return an empty body — guard against that.
  const text = await res.text();
  return text ? (JSON.parse(text) as T) : (undefined as T);
}
