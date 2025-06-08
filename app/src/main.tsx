import { StrictMode } from 'react'
import { createRoot } from 'react-dom/client'
import { createBrowserRouter, RouterProvider } from 'react-router-dom'
import './index.css'
import VideoList from './Video_list.tsx'
import VideoPlay from './Video_play.tsx'

const router = createBrowserRouter([
  {
    path: "/",
    element: <VideoList />,
  },
  {
    path: "/play/:id",
    element: <VideoPlay />,
  },
]);

createRoot(document.getElementById('root')!).render(
  <StrictMode>
    <RouterProvider router={router} />
  </StrictMode>,
)
